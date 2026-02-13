#pragma once

#include "CoreMinimal.h"
#include "UTAAgentContracts.h"

class UTACORE_API FUTAReadFileTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class UTACORE_API FUTASearchFilesTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class UTACORE_API FUTAWriteFileTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};
