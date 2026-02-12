#pragma once

#include "CoreMinimal.h"
#include "UTAAgentContracts.h"

class FUTAListBlueprintsTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class FUTAReadBlueprintMetadataTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class FUTACreateBlueprintAssetTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};
