#pragma once

#include "CoreMinimal.h"
#include "UTAAgentContracts.h"
#include "UTAProviderSettings.h"

class FUTADeepSeekProvider : public IUTALLMProvider
{
public:
    virtual FString GetProviderId() const override;
    virtual void Configure(const FString& ApiKey, const FString& BaseUrl, const FString& Model) override;
    virtual bool GenerateResponse(const FString& UserPrompt, FString& OutResponse, FString& OutError) override;

    void Configure(const FUTAProviderSettings& InSettings);

private:
    FUTAProviderSettings Settings;
};
