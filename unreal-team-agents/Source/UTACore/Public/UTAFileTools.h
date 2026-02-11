#pragma once

#include "CoreMinimal.h"
#include "UTAAgentContracts.h"

class FUTAReadFileTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class FUTASearchFilesTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class FUTAWriteFileTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};
