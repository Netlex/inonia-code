#pragma once

#include "Modules/ModuleManager.h"

class FUTAChatOrchestrator;

class FUTAChatModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    TSharedPtr<FUTAChatOrchestrator> GetOrchestrator() const;

private:
    TSharedPtr<FUTAChatOrchestrator> Orchestrator;
};
