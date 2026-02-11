#pragma once

#include "Modules/ModuleManager.h"

class FUTABlueprintModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
