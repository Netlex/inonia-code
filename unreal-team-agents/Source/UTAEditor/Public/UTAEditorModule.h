#pragma once

#include "Modules/ModuleManager.h"

class FUTAEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    static const FName MainTabId;
    void RegisterTabSpawner();
    void UnregisterTabSpawner();

    TSharedRef<class SDockTab> SpawnMainTab(const class FSpawnTabArgs& SpawnTabArgs) const;
};
