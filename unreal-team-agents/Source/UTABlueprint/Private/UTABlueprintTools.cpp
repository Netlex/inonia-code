#include "UTABlueprintTools.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "GameFramework/Actor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Serialization/JsonSerializer.h"

namespace
{
bool TryParseArgs(const FString& JsonArguments, TSharedPtr<FJsonObject>& OutJson)
{
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonArguments);
    return FJsonSerializer::Deserialize(Reader, OutJson) && OutJson.IsValid();
}

FString ToJsonString(const TSharedRef<FJsonObject>& JsonObject)
{
    FString Out;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
    FJsonSerializer::Serialize(JsonObject, Writer);
    return Out;
}

FUTAToolResult MakeErrorResult(const FString& Message)
{
    FUTAToolResult Result;
    Result.ErrorMessage = Message;

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("error"), Message);
    Result.JsonPayload = ToJsonString(Json);

    return Result;
}

UClass* ResolveParentClass(const FString& ParentClassName)
{
    if (ParentClassName.IsEmpty())
    {
        return AActor::StaticClass();
    }

    UClass* ParentClass = FindObject<UClass>(ANY_PACKAGE, *ParentClassName);
    return ParentClass ? ParentClass : AActor::StaticClass();
}
} // namespace

FString FUTAListBlueprintsTool::GetName() const
{
    return TEXT("list_blueprints");
}

FUTAToolResult FUTAListBlueprintsTool::Execute(const FString& JsonArguments)
{
    TSharedPtr<FJsonObject> Args;
    const bool bHasArgs = !JsonArguments.IsEmpty();
    if (bHasArgs && !TryParseArgs(JsonArguments, Args))
    {
        return MakeErrorResult(TEXT("Invalid JSON arguments"));
    }

    FString PackagePath = TEXT("/Game");
    if (Args.IsValid())
    {
        Args->TryGetStringField(TEXT("packagePath"), PackagePath);
    }

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    FARFilter Filter;
    Filter.PackagePaths.Add(*PackagePath);
    Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
    Filter.bRecursivePaths = true;

    TArray<FAssetData> Assets;
    AssetRegistryModule.Get().GetAssets(Filter, Assets);

    TArray<TSharedPtr<FJsonValue>> AssetValues;
    for (const FAssetData& Asset : Assets)
    {
        AssetValues.Add(MakeShared<FJsonValueString>(Asset.GetSoftObjectPath().ToString()));
    }

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("packagePath"), PackagePath);
    Json->SetArrayField(TEXT("blueprints"), AssetValues);

    FUTAToolResult Result;
    Result.bSuccess = true;
    Result.JsonPayload = ToJsonString(Json);
    return Result;
}

FString FUTAReadBlueprintMetadataTool::GetName() const
{
    return TEXT("read_blueprint_metadata");
}

FUTAToolResult FUTAReadBlueprintMetadataTool::Execute(const FString& JsonArguments)
{
    TSharedPtr<FJsonObject> Args;
    if (!TryParseArgs(JsonArguments, Args))
    {
        return MakeErrorResult(TEXT("Invalid JSON arguments"));
    }

    FString AssetPath;
    if (!Args->TryGetStringField(TEXT("assetPath"), AssetPath))
    {
        return MakeErrorResult(TEXT("Missing 'assetPath'"));
    }

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(AssetPath));

    if (!AssetData.IsValid())
    {
        return MakeErrorResult(FString::Printf(TEXT("Blueprint asset not found: %s"), *AssetPath));
    }

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("assetPath"), AssetData.GetSoftObjectPath().ToString());
    Json->SetStringField(TEXT("assetName"), AssetData.AssetName.ToString());
    Json->SetStringField(TEXT("packageName"), AssetData.PackageName.ToString());
    Json->SetStringField(TEXT("assetClass"), AssetData.AssetClassPath.ToString());

    TSharedRef<FJsonObject> TagsJson = MakeShared<FJsonObject>();
    for (const TPair<FName, FAssetTagValueRef>& TagPair : AssetData.TagsAndValues)
    {
        TagsJson->SetStringField(TagPair.Key.ToString(), TagPair.Value.AsString());
    }
    Json->SetObjectField(TEXT("tags"), TagsJson);

    FUTAToolResult Result;
    Result.bSuccess = true;
    Result.JsonPayload = ToJsonString(Json);
    return Result;
}


FString FUTAGetBlueprintGraphSummaryTool::GetName() const
{
    return TEXT("get_blueprint_graph_summary");
}

FUTAToolResult FUTAGetBlueprintGraphSummaryTool::Execute(const FString& JsonArguments)
{
    TSharedPtr<FJsonObject> Args;
    if (!TryParseArgs(JsonArguments, Args))
    {
        return MakeErrorResult(TEXT("Invalid JSON arguments"));
    }

    FString AssetPath;
    if (!Args->TryGetStringField(TEXT("assetPath"), AssetPath))
    {
        return MakeErrorResult(TEXT("Missing 'assetPath'"));
    }

    UObject* LoadedObject = StaticLoadObject(UBlueprint::StaticClass(), nullptr, *AssetPath);
    UBlueprint* Blueprint = Cast<UBlueprint>(LoadedObject);
    if (!Blueprint)
    {
        return MakeErrorResult(FString::Printf(TEXT("Failed to load Blueprint: %s"), *AssetPath));
    }

    TArray<TSharedPtr<FJsonValue>> GraphValues;

    auto AddGraphSummary = [&GraphValues](const UEdGraph* Graph, const FString& GraphType)
    {
        if (!Graph)
        {
            return;
        }

        int32 NodeCount = 0;
        for (const UEdGraphNode* Node : Graph->Nodes)
        {
            if (Node)
            {
                ++NodeCount;
            }
        }

        TSharedRef<FJsonObject> GraphJson = MakeShared<FJsonObject>();
        GraphJson->SetStringField(TEXT("graphType"), GraphType);
        GraphJson->SetStringField(TEXT("graphName"), Graph->GetName());
        GraphJson->SetNumberField(TEXT("nodeCount"), NodeCount);
        GraphValues.Add(MakeShared<FJsonValueObject>(GraphJson));
    };

    for (const UEdGraph* Graph : Blueprint->UbergraphPages)
    {
        AddGraphSummary(Graph, TEXT("ubergraph"));
    }

    for (const UEdGraph* Graph : Blueprint->FunctionGraphs)
    {
        AddGraphSummary(Graph, TEXT("function"));
    }

    for (const UEdGraph* Graph : Blueprint->MacroGraphs)
    {
        AddGraphSummary(Graph, TEXT("macro"));
    }

    for (const UEdGraph* Graph : Blueprint->EventGraphs)
    {
        AddGraphSummary(Graph, TEXT("event"));
    }

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("assetPath"), AssetPath);
    Json->SetArrayField(TEXT("graphs"), GraphValues);

    FUTAToolResult Result;
    Result.bSuccess = true;
    Result.JsonPayload = ToJsonString(Json);
    return Result;
}

FString FUTACreateBlueprintAssetTool::GetName() const
{
    return TEXT("create_blueprint_asset");
}

FUTAToolResult FUTACreateBlueprintAssetTool::Execute(const FString& JsonArguments)
{
    TSharedPtr<FJsonObject> Args;
    if (!TryParseArgs(JsonArguments, Args))
    {
        return MakeErrorResult(TEXT("Invalid JSON arguments"));
    }

    FString PackagePath;
    FString AssetName;
    FString ParentClassName;

    if (!Args->TryGetStringField(TEXT("packagePath"), PackagePath) || !Args->TryGetStringField(TEXT("assetName"), AssetName))
    {
        return MakeErrorResult(TEXT("Missing 'packagePath' or 'assetName'"));
    }

    Args->TryGetStringField(TEXT("parentClass"), ParentClassName);

    UClass* ParentClass = ResolveParentClass(ParentClassName);
    if (!ParentClass)
    {
        return MakeErrorResult(TEXT("Unable to resolve parent class"));
    }

    FString NormalizedPackagePath = PackagePath;
    if (!NormalizedPackagePath.StartsWith(TEXT("/")))
    {
        NormalizedPackagePath = TEXT("/") + NormalizedPackagePath;
    }

    const FString FullPackageName = NormalizedPackagePath / AssetName;
    UPackage* Package = CreatePackage(*FullPackageName);
    if (!Package)
    {
        return MakeErrorResult(FString::Printf(TEXT("Failed to create package: %s"), *FullPackageName));
    }

    UBlueprint* NewBlueprint = FKismetEditorUtilities::CreateBlueprint(
        ParentClass,
        Package,
        *AssetName,
        BPTYPE_Normal,
        UBlueprint::StaticClass(),
        UBlueprintGeneratedClass::StaticClass(),
        FName(TEXT("UTA")));

    if (!NewBlueprint)
    {
        return MakeErrorResult(TEXT("Failed to create blueprint asset"));
    }

    FAssetRegistryModule::AssetCreated(NewBlueprint);
    NewBlueprint->MarkPackageDirty();

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("assetPath"), NewBlueprint->GetPathName());
    Json->SetStringField(TEXT("package"), FullPackageName);
    Json->SetStringField(TEXT("parentClass"), ParentClass->GetName());

    FUTAToolResult Result;
    Result.bSuccess = true;
    Result.JsonPayload = ToJsonString(Json);
    return Result;
}
