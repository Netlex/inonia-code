#include "UTAEditorModule.h"

#include "LevelEditor.h"
#include "SUTAChatPanel.h"
#include "Widgets/Docking/SDockTab.h"

const FName FUTAEditorModule::MainTabId(TEXT("UTA.Main"));

IMPLEMENT_MODULE(FUTAEditorModule, UTAEditor)

void FUTAEditorModule::StartupModule()
{
    RegisterTabSpawner();
}

void FUTAEditorModule::ShutdownModule()
{
    UnregisterTabSpawner();
}

void FUTAEditorModule::RegisterTabSpawner()
{
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        MainTabId,
        FOnSpawnTab::CreateRaw(this, &FUTAEditorModule::SpawnMainTab))
        .SetDisplayName(FText::FromString(TEXT("Unreal Team Agents")))
        .SetTooltipText(FText::FromString(TEXT("Open Unreal Team Agents chat panel")));
}

void FUTAEditorModule::UnregisterTabSpawner()
{
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MainTabId);
}

TSharedRef<SDockTab> FUTAEditorModule::SpawnMainTab(const FSpawnTabArgs& SpawnTabArgs) const
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SUTAChatPanel)
        ];
}
