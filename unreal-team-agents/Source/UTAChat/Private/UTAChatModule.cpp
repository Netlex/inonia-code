#include "UTAChatModule.h"

#include "Modules/ModuleManager.h"
#include "Misc/ConfigCacheIni.h"
#include "HAL/PlatformMisc.h"

#include "UTAChatOrchestrator.h"
#include "UTADeepSeekProvider.h"
#include "UTAFileTools.h"
#include "UTAInMemoryConversationStore.h"
#include "UTAProviderSettings.h"
#include "UTAToolRegistry.h"

namespace
{
FUTAProviderSettings LoadProviderSettings()
{
    FUTAProviderSettings Settings;

    if (GConfig)
    {
        GConfig->GetString(TEXT("UTA.DeepSeek"), TEXT("ApiKey"), Settings.ApiKey, GGameIni);
        GConfig->GetString(TEXT("UTA.DeepSeek"), TEXT("BaseUrl"), Settings.BaseUrl, GGameIni);
        GConfig->GetString(TEXT("UTA.DeepSeek"), TEXT("ChatEndpoint"), Settings.ChatEndpoint, GGameIni);
        GConfig->GetString(TEXT("UTA.DeepSeek"), TEXT("Model"), Settings.Model, GGameIni);
        GConfig->GetFloat(TEXT("UTA.DeepSeek"), TEXT("TimeoutSeconds"), Settings.TimeoutSeconds, GGameIni);
    }

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
