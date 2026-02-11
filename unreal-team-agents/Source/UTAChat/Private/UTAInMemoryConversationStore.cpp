#include "UTAInMemoryConversationStore.h"

void FUTAInMemoryConversationStore::AppendUserMessage(const FString& Message)
{
    Transcript.Add(FString::Printf(TEXT("user: %s"), *Message));
}

void FUTAInMemoryConversationStore::AppendAssistantMessage(const FString& Message)
{
    Transcript.Add(FString::Printf(TEXT("assistant: %s"), *Message));
}

TArray<FString> FUTAInMemoryConversationStore::GetTranscript() const
{
    return Transcript;
}
