#pragma once

#include "CoreMinimal.h"
#include "UTAAgentContracts.h"

class UTACORE_API FUTAToolRegistry : public IUTAToolRegistry
{
public:
    virtual void RegisterTool(TSharedRef<IUTATool> Tool) override;
    virtual bool HasTool(const FString& ToolName) const override;
    virtual FUTAToolResult ExecuteTool(const FString& ToolName, const FString& JsonArguments) const override;
    virtual TArray<FString> ListTools() const override;

private:
    TMap<FString, TSharedRef<IUTATool>> Tools;
};
