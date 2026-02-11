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

private:
    TSharedRef<IUTALLMProvider> Provider;
    TSharedRef<IUTAConversationStore> ConversationStore;
    TSharedPtr<IUTAToolRegistry> ToolRegistry;

    bool TryHandleToolCommand(const FString& Message, FString& OutAssistantResponse, FString& OutError);
};
