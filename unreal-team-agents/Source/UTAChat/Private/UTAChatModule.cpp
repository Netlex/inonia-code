#include "UTAChatModule.h"

#include "Modules/ModuleManager.h"
#include "HAL/PlatformMisc.h"

#include "UTAChatOrchestrator.h"
#include "UTADeepSeekProvider.h"
#include "UTAFileTools.h"
#include "UTAInMemoryConversationStore.h"
#include "UTAProjectSettings.h"
#include "UTAProviderSettings.h"
#include "UTAToolRegistry.h"

namespace
{
FUTAProviderSettings LoadProviderSettings()
{
    FUTAProviderSettings Settings;

    const UUTAProjectSettings* ProjectSettings = GetDefault<UUTAProjectSettings>();
    if (ProjectSettings)
    {
        Settings.TimeoutSeconds = ProjectSettings->ProviderTimeoutSeconds;

        FString ActiveProviderId = TEXT("deepseek");
        if (!ProjectSettings->ActiveAgentId.IsEmpty())
        {
            for (const FUTAAgentProfile& Agent : ProjectSettings->Agents)
            {
                if (Agent.AgentId == ProjectSettings->ActiveAgentId)
                {
                    ActiveProviderId = Agent.ProviderId;
                    break;
                }
            }
        }

        for (const FUTAProviderProfile& ProviderProfile : ProjectSettings->Providers)
        {
            if (ProviderProfile.ProviderId == ActiveProviderId)
            {
                Settings.ApiKey = ProviderProfile.ApiKey;
                Settings.BaseUrl = ProviderProfile.BaseUrl;
                Settings.Model = ProviderProfile.Model;
                break;
            }
        }
    }

    // Environment overrides remain available for CI/dev usage.
    const FString EnvApiKey = FPlatformMisc::GetEnvironmentVariable(TEXT("UTA_DEEPSEEK_API_KEY"));
    if (!EnvApiKey.IsEmpty())
    {
        Settings.ApiKey = EnvApiKey;
    }

    const FString EnvBaseUrl = FPlatformMisc::GetEnvironmentVariable(TEXT("UTA_DEEPSEEK_BASE_URL"));
    if (!EnvBaseUrl.IsEmpty())
    {
        Settings.BaseUrl = EnvBaseUrl;
    }

    const FString EnvModel = FPlatformMisc::GetEnvironmentVariable(TEXT("UTA_DEEPSEEK_MODEL"));
    if (!EnvModel.IsEmpty())
    {
        Settings.Model = EnvModel;
    }

    return Settings;
}

FString LoadActiveAgentSystemPrompt()
{
    const UUTAProjectSettings* ProjectSettings = GetDefault<UUTAProjectSettings>();
    if (!ProjectSettings)
    {
        return TEXT("You are Unreal Team Agents assistant.");
    }

    for (const FUTAAgentProfile& Agent : ProjectSettings->Agents)
    {
        if (Agent.AgentId == ProjectSettings->ActiveAgentId)
        {
            return Agent.SystemPrompt.IsEmpty() ? TEXT("You are Unreal Team Agents assistant.") : Agent.SystemPrompt;
        }
    }

    return TEXT("You are Unreal Team Agents assistant.");
}
} // namespace

IMPLEMENT_MODULE(FUTAChatModule, UTAChat)

void FUTAChatModule::StartupModule()
{
    TSharedRef<FUTADeepSeekProvider> Provider = MakeShared<FUTADeepSeekProvider>();
    Provider->Configure(LoadProviderSettings());

    TSharedRef<FUTAInMemoryConversationStore> ConversationStore = MakeShared<FUTAInMemoryConversationStore>();
    ToolRegistry = MakeShared<FUTAToolRegistry>();

    ToolRegistry->RegisterTool(MakeShared<FUTAReadFileTool>());
    ToolRegistry->RegisterTool(MakeShared<FUTASearchFilesTool>());
    ToolRegistry->RegisterTool(MakeShared<FUTAWriteFileTool>());

    Orchestrator = MakeShared<FUTAChatOrchestrator>(Provider, ConversationStore, ToolRegistry);
    Orchestrator->SetSystemPrompt(LoadActiveAgentSystemPrompt());
}

void FUTAChatModule::ShutdownModule()
{
    Orchestrator.Reset();
    ToolRegistry.Reset();
}

TSharedPtr<FUTAChatOrchestrator> FUTAChatModule::GetOrchestrator() const
{
    return Orchestrator;
}

bool FUTAChatModule::RegisterTool(TSharedRef<IUTATool> Tool)
{
    if (!ToolRegistry.IsValid())
    {
        return false;
    }

    ToolRegistry->RegisterTool(Tool);
    return true;
}
