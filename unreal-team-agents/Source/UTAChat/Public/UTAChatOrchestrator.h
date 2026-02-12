#pragma once

#include "CoreMinimal.h"
#include "UTAAgentContracts.h"

class FUTAChatOrchestrator : public IUTAAgentOrchestrator
{
public:
    FUTAChatOrchestrator(
        TSharedRef<IUTALLMProvider> InProvider,
        TSharedRef<IUTAConversationStore> InConversationStore,
        TSharedPtr<IUTAToolRegistry> InToolRegistry);

    virtual bool ProcessUserMessage(const FString& Message, FString& OutAssistantResponse, FString& OutError) override;

    void SetSystemPrompt(const FString& InSystemPrompt);

private:
    TSharedRef<IUTALLMProvider> Provider;
    TSharedRef<IUTAConversationStore> ConversationStore;
    TSharedPtr<IUTAToolRegistry> ToolRegistry;
    FString SystemPrompt = TEXT("You are Unreal Team Agents assistant.");

    bool TryHandleToolCommand(const FString& Message, FString& OutAssistantResponse, FString& OutError);
    bool TryAutoExecuteToolFromNaturalLanguage(const FString& Message, FString& OutAssistantResponse, FString& OutError);
};
