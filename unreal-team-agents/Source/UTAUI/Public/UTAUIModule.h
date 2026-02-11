#pragma once

#include "Modules/ModuleManager.h"

class FUTAUIModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
