#include "UTAChatModule.h"

#include "UTAChatOrchestrator.h"
#include "UTADeepSeekProvider.h"
#include "UTAInMemoryConversationStore.h"
#include "UTAFileTools.h"
#include "UTAToolRegistry.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FUTAChatModule, UTAChat)

void FUTAChatModule::StartupModule()
{
    TSharedRef<FUTADeepSeekProvider> Provider = MakeShared<FUTADeepSeekProvider>();
    TSharedRef<FUTAInMemoryConversationStore> ConversationStore = MakeShared<FUTAInMemoryConversationStore>();
    TSharedRef<FUTAToolRegistry> ToolRegistry = MakeShared<FUTAToolRegistry>();

    ToolRegistry->RegisterTool(MakeShared<FUTAReadFileTool>());
    ToolRegistry->RegisterTool(MakeShared<FUTASearchFilesTool>());
    ToolRegistry->RegisterTool(MakeShared<FUTAWriteFileTool>());

    Orchestrator = MakeShared<FUTAChatOrchestrator>(Provider, ConversationStore, ToolRegistry);
}

void FUTAChatModule::ShutdownModule()
{
    Orchestrator.Reset();
}

TSharedPtr<FUTAChatOrchestrator> FUTAChatModule::GetOrchestrator() const
{
    return Orchestrator;
}
