#pragma once

#include "CoreMinimal.h"
#include "UTAAgentContracts.h"

class FUTAInMemoryConversationStore : public IUTAConversationStore
{
public:
    virtual void AppendUserMessage(const FString& Message) override;
    virtual void AppendAssistantMessage(const FString& Message) override;
    virtual TArray<FString> GetTranscript() const override;

private:
    TArray<FString> Transcript;
};
