#pragma once

#include "CoreMinimal.h"
#include "UTAAgentContracts.h"

class UTABLUEPRINT_API FUTAListBlueprintsTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class UTABLUEPRINT_API FUTAReadBlueprintMetadataTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class UTABLUEPRINT_API FUTAGetBlueprintGraphSummaryTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};

class UTABLUEPRINT_API FUTACreateBlueprintAssetTool : public IUTATool
{
public:
    virtual FString GetName() const override;
    virtual FUTAToolResult Execute(const FString& JsonArguments) override;
};
