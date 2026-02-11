#pragma once

#include "Modules/ModuleManager.h"

class FUTACoreModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
