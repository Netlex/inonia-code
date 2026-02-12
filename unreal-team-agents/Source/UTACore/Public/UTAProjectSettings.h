#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UTAProjectSettings.generated.h"

USTRUCT(BlueprintType)
struct FUTAProviderProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Config, Category = "Provider")
    FString ProviderId = TEXT("deepseek");

    UPROPERTY(EditAnywhere, Config, Category = "Provider")
    FString BaseUrl = TEXT("https://api.deepseek.com");

    UPROPERTY(EditAnywhere, Config, Category = "Provider")
    FString Model = TEXT("deepseek-chat");

    UPROPERTY(EditAnywhere, Config, Category = "Provider")
    FString ApiKey;
};

USTRUCT(BlueprintType)
struct FUTAAgentProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Config, Category = "Agent")
    FString AgentId = TEXT("default");

    UPROPERTY(EditAnywhere, Config, Category = "Agent")
    FString DisplayName = TEXT("General Agent");

    UPROPERTY(EditAnywhere, Config, Category = "Agent", meta = (MultiLine = "true"))
    FString SystemPrompt = TEXT("You are Unreal Team Agents assistant.");

    UPROPERTY(EditAnywhere, Config, Category = "Agent")
    FString ProviderId = TEXT("deepseek");
};

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Unreal Team Agents"))
class UTACore_API UUTAProjectSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UUTAProjectSettings();
    virtual FName GetCategoryName() const override;

    UPROPERTY(EditAnywhere, Config, Category = "General")
    FString ActiveAgentId = TEXT("default");

    UPROPERTY(EditAnywhere, Config, Category = "General")
    float ProviderTimeoutSeconds = 60.0f;

    UPROPERTY(EditAnywhere, Config, Category = "General")
    bool bEnableAutoToolRouting = true;

    UPROPERTY(EditAnywhere, Config, Category = "Providers")
    TArray<FUTAProviderProfile> Providers;

    UPROPERTY(EditAnywhere, Config, Category = "Agents")
    TArray<FUTAAgentProfile> Agents;
};
