#pragma once

#include "Modules/ModuleManager.h"

class FUTAChatModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
