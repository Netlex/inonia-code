#pragma once

#include "Modules/ModuleManager.h"
#include "UTAAgentContracts.h"

class FUTAChatOrchestrator;

class FUTAChatModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    TSharedPtr<FUTAChatOrchestrator> GetOrchestrator() const;

    bool RegisterTool(TSharedRef<IUTATool> Tool);

private:
    TSharedPtr<FUTAChatOrchestrator> Orchestrator;
    TSharedPtr<IUTAToolRegistry> ToolRegistry;
};
