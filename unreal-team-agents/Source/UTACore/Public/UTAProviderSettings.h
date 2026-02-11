#pragma once

#include "CoreMinimal.h"

struct FUTAProviderSettings
{
    FString ApiKey;
    FString BaseUrl = TEXT("https://api.deepseek.com");
    FString ChatEndpoint = TEXT("/chat/completions");
    FString Model = TEXT("deepseek-chat");
    float TimeoutSeconds = 60.0f;
};
