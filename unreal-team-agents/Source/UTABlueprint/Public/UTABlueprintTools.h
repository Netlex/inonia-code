#pragma once

#include "CoreMinimal.h"
#include "UTAAgentContracts.h"

class UTABlueprint_API FUTAListBlueprintsTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class UTABlueprint_API FUTAReadBlueprintMetadataTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class UTABlueprint_API FUTAGetBlueprintGraphSummaryTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class UTABlueprint_API FUTACreateBlueprintAssetTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};
