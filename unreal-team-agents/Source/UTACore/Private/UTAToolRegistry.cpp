#include "UTAToolRegistry.h"

void FUTAToolRegistry::RegisterTool(TSharedRef<IUTATool> Tool)
{
    const FString Name = Tool->GetName();
    Tools.Add(Name, Tool);
}

bool FUTAToolRegistry::HasTool(const FString& ToolName) const
{
    return Tools.Contains(ToolName);
}

FUTAToolResult FUTAToolRegistry::ExecuteTool(const FString& ToolName, const FString& JsonArguments) const
{
    const TSharedRef<IUTATool>* Tool = Tools.Find(ToolName);
    if (!Tool)
    {
        FUTAToolResult Result;
        Result.bSuccess = false;
        Result.ErrorMessage = FString::Printf(TEXT("Tool '%s' not found"), *ToolName);
        return Result;
    }

    return (*Tool)->Execute(JsonArguments);
}

TArray<FString> FUTAToolRegistry::ListTools() const
{
    TArray<FString> Names;
    Tools.GetKeys(Names);
    Names.Sort();
    return Names;
}
