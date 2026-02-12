#include "UTABlueprintModule.h"

#include "Modules/ModuleManager.h"

#include "UTAChatModule.h"
#include "UTABlueprintTools.h"

IMPLEMENT_MODULE(FUTABlueprintModule, UTABlueprint)

void FUTABlueprintModule::StartupModule()
{
    if (FUTAChatModule* ChatModule = FModuleManager::LoadModulePtr<FUTAChatModule>(TEXT("UTAChat")))
    {
        ChatModule->RegisterTool(MakeShared<FUTAListBlueprintsTool>());
        ChatModule->RegisterTool(MakeShared<FUTAReadBlueprintMetadataTool>());
        ChatModule->RegisterTool(MakeShared<FUTAGetBlueprintGraphSummaryTool>());
        ChatModule->RegisterTool(MakeShared<FUTACreateBlueprintAssetTool>());
    }
}

void FUTABlueprintModule::ShutdownModule()
{
}
