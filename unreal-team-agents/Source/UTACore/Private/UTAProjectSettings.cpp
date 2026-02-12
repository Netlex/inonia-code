#include "UTAProjectSettings.h"

UUTAProjectSettings::UUTAProjectSettings()
{
    if (Providers.Num() == 0)
    {
        FUTAProviderProfile Provider;
        Provider.ProviderId = TEXT("deepseek");
        Provider.BaseUrl = TEXT("https://api.deepseek.com");
        Provider.Model = TEXT("deepseek-chat");
        Providers.Add(Provider);
    }

    if (Agents.Num() == 0)
    {
        FUTAAgentProfile Agent;
        Agent.AgentId = TEXT("default");
        Agent.DisplayName = TEXT("General Agent");
        Agent.SystemPrompt = TEXT("You are Unreal Team Agents assistant. Decide which tools to call when needed and provide concise results.");
        Agent.ProviderId = TEXT("deepseek");
        Agents.Add(Agent);
    }
}

FName UUTAProjectSettings::GetCategoryName() const
{
    return TEXT("Plugins");
}
