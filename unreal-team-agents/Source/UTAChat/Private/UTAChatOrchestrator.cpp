#include "UTAChatOrchestrator.h"

#include "Misc/DefaultValueHelper.h"

FUTAChatOrchestrator::FUTAChatOrchestrator(
    TSharedRef<IUTALLMProvider> InProvider,
    TSharedRef<IUTAConversationStore> InConversationStore,
    TSharedPtr<IUTAToolRegistry> InToolRegistry)
    : Provider(InProvider), ConversationStore(InConversationStore), ToolRegistry(MoveTemp(InToolRegistry))
{
}

bool FUTAChatOrchestrator::ProcessUserMessage(const FString& Message, FString& OutAssistantResponse, FString& OutError)
{
    ConversationStore->AppendUserMessage(Message);

    if (TryHandleToolCommand(Message, OutAssistantResponse, OutError))
    {
        ConversationStore->AppendAssistantMessage(OutAssistantResponse);
        return true;
    }

    if (!Provider->GenerateResponse(Message, OutAssistantResponse, OutError))
    {
        return false;
    }

    ConversationStore->AppendAssistantMessage(OutAssistantResponse);
    return true;
}

bool FUTAChatOrchestrator::TryHandleToolCommand(const FString& Message, FString& OutAssistantResponse, FString& OutError)
{
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
