#include "UTAChatOrchestrator.h"

#include "Serialization/JsonSerializer.h"

namespace
{
FString BuildToolListResponse(const TArray<FString>& ToolNames)
{
    TArray<TSharedPtr<FJsonValue>> ToolValues;
    for (const FString& Name : ToolNames)
    {
        ToolValues.Add(MakeShared<FJsonValueString>(Name));
    }

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetArrayField(TEXT("tools"), ToolValues);

    FString Payload;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Payload);
    FJsonSerializer::Serialize(Json, Writer);
    return Payload;
}

bool ParseRouterResponse(const FString& Payload, FString& OutToolName, FString& OutArgsJson)
{
    TSharedPtr<FJsonObject> Json;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Payload);
    if (!FJsonSerializer::Deserialize(Reader, Json) || !Json.IsValid())
    {
        return false;
    }

    if (!Json->TryGetStringField(TEXT("tool"), OutToolName))
    {
        return false;
    }

    const TSharedPtr<FJsonValue>* ArgsValue = Json->Values.Find(TEXT("args"));
    TSharedPtr<FJsonObject> ArgsObj = ArgsValue && ArgsValue->IsValid() ? (*ArgsValue)->AsObject() : nullptr;
    if (!ArgsObj.IsValid())
    {
        OutArgsJson = TEXT("{}");
        return true;
    }

    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutArgsJson);
    FJsonSerializer::Serialize(ArgsObj.ToSharedRef(), Writer);
    return true;
}
} // namespace

FUTAChatOrchestrator::FUTAChatOrchestrator(
    TSharedRef<IUTALLMProvider> InProvider,
    TSharedRef<IUTAConversationStore> InConversationStore,
    TSharedPtr<IUTAToolRegistry> InToolRegistry)
    : Provider(InProvider), ConversationStore(InConversationStore), ToolRegistry(MoveTemp(InToolRegistry))
{
}

void FUTAChatOrchestrator::SetSystemPrompt(const FString& InSystemPrompt)
{
    SystemPrompt = InSystemPrompt;
}

bool FUTAChatOrchestrator::ProcessUserMessage(const FString& Message, FString& OutAssistantResponse, FString& OutError)
{
    ConversationStore->AppendUserMessage(Message);

    if (TryHandleToolCommand(Message, OutAssistantResponse, OutError))
    {
        ConversationStore->AppendAssistantMessage(OutAssistantResponse);
        return true;
    }

    if (TryAutoExecuteToolFromNaturalLanguage(Message, OutAssistantResponse, OutError))
    {
        ConversationStore->AppendAssistantMessage(OutAssistantResponse);
        return true;
    }

    const FString Prompt = SystemPrompt + TEXT("\n\nUser request:\n") + Message;
    if (!Provider->GenerateResponse(Prompt, OutAssistantResponse, OutError))
    {
        return false;
    }

    ConversationStore->AppendAssistantMessage(OutAssistantResponse);
    return true;
}

bool FUTAChatOrchestrator::TryHandleToolCommand(const FString& Message, FString& OutAssistantResponse, FString& OutError)
{
    if (Message.Equals(TEXT("/tools"), ESearchCase::IgnoreCase))
    {
        if (!ToolRegistry.IsValid())
        {
            OutError = TEXT("Tool registry is not configured");
            return false;
        }

        OutAssistantResponse = BuildToolListResponse(ToolRegistry->ListTools());
        return true;
    }

    // Minimal tool protocol for MVP:
    // /tool <name> <json-args>
    if (!Message.StartsWith(TEXT("/tool ")))
    {
        return false;
    }

    if (!ToolRegistry.IsValid())
    {
        OutError = TEXT("Tool registry is not configured");
        return false;
    }

    FString CommandPayload = Message.RightChop(6).TrimStartAndEnd();
    FString ToolName;
    FString JsonArguments;

    if (!CommandPayload.Split(TEXT(" "), &ToolName, &JsonArguments))
    {
        OutError = TEXT("Invalid tool command format. Use: /tool <name> <json-args>");
        return false;
    }

    const FUTAToolResult ToolResult = ToolRegistry->ExecuteTool(ToolName, JsonArguments);
    if (!ToolResult.bSuccess)
    {
        OutError = ToolResult.ErrorMessage;
        OutAssistantResponse = ToolResult.JsonPayload;
        return true;
    }

    OutAssistantResponse = ToolResult.JsonPayload;
    return true;
}

bool FUTAChatOrchestrator::TryAutoExecuteToolFromNaturalLanguage(const FString& Message, FString& OutAssistantResponse, FString& OutError)
{
    if (!ToolRegistry.IsValid())
    {
        return false;
    }

    const TArray<FString> ToolNames = ToolRegistry->ListTools();
    FString ToolList;
    for (int32 i = 0; i < ToolNames.Num(); ++i)
    {
        ToolList += ToolNames[i];
        if (i + 1 < ToolNames.Num())
        {
            ToolList += TEXT(", ");
        }
    }

    const FString RouterPrompt = FString::Printf(
        TEXT("You are a tool router for Unreal Team Agents. Available tools: [%s]. "
             "Given user request, return only strict JSON: {\"tool\":\"<name or none>\",\"args\":{...}}. "
             "If no tool fits, return {\"tool\":\"none\",\"args\":{}}. "
             "User request: %s"),
        *ToolList,
        *Message);

    FString RouterResponse;
    FString RouterError;
    if (!Provider->GenerateResponse(RouterPrompt, RouterResponse, RouterError))
    {
        return false;
    }

    FString ToolName;
    FString ArgsJson;
    if (!ParseRouterResponse(RouterResponse, ToolName, ArgsJson))
    {
        return false;
    }

    if (ToolName.Equals(TEXT("none"), ESearchCase::IgnoreCase))
    {
        return false;
    }

    if (!ToolRegistry->HasTool(ToolName))
    {
        return false;
    }

    const FUTAToolResult ToolResult = ToolRegistry->ExecuteTool(ToolName, ArgsJson);
    if (!ToolResult.bSuccess)
    {
        OutError = ToolResult.ErrorMessage;
        OutAssistantResponse = ToolResult.JsonPayload;
        return true;
    }

    OutAssistantResponse = ToolResult.JsonPayload;
    return true;
}
