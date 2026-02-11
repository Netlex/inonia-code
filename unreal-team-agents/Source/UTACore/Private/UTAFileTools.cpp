#include "UTAFileTools.h"

#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "UTAExecutionPolicy.h"

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

FString MakeErrorJson(const FString& Message)
{
    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("error"), Message);
    return ToJsonString(Json);
}

FString ResolveWorkspacePath(const FString& RelativeOrAbsolutePath)
{
    if (FPaths::IsRelative(RelativeOrAbsolutePath))
    {
        return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), RelativeOrAbsolutePath);
    }

    return FPaths::ConvertRelativePathToFull(RelativeOrAbsolutePath);
}

FUTAToolResult MakeErrorResult(const FString& Message)
{
    FUTAToolResult Result;
    Result.ErrorMessage = Message;
    Result.JsonPayload = MakeErrorJson(Message);
    return Result;
}
} // namespace

FString FUTAReadFileTool::GetName() const
{
    return TEXT("read_file");
}

FUTAToolResult FUTAReadFileTool::Execute(const FString& JsonArguments)
{
    TSharedPtr<FJsonObject> Args;
    if (!TryParseArgs(JsonArguments, Args))
    {
        return MakeErrorResult(TEXT("Invalid JSON arguments"));
    }

    FString Path;
    if (!Args->TryGetStringField(TEXT("path"), Path))
    {
        return MakeErrorResult(TEXT("Missing 'path'"));
    }

    const FString FullPath = ResolveWorkspacePath(Path);
    FUTAExecutionPolicy Policy;

    if (!Policy.CanReadPath(FullPath))
    {
        return MakeErrorResult(FString::Printf(TEXT("Read access denied for path: %s"), *FullPath));
    }

    FString Content;
    if (!FFileHelper::LoadFileToString(Content, *FullPath))
    {
        return MakeErrorResult(FString::Printf(TEXT("Failed to read file: %s"), *FullPath));
    }

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("path"), FullPath);
    Json->SetStringField(TEXT("content"), Content);

    FUTAToolResult Result;
    Result.bSuccess = true;
    Result.JsonPayload = ToJsonString(Json);
    return Result;
}

FString FUTASearchFilesTool::GetName() const
{
    return TEXT("search_files");
}

FUTAToolResult FUTASearchFilesTool::Execute(const FString& JsonArguments)
{
    TSharedPtr<FJsonObject> Args;
    if (!TryParseArgs(JsonArguments, Args))
    {
        return MakeErrorResult(TEXT("Invalid JSON arguments"));
    }

    FString Query;
    if (!Args->TryGetStringField(TEXT("query"), Query) || Query.IsEmpty())
    {
        return MakeErrorResult(TEXT("Missing or empty 'query'"));
    }

    FString RootDir = FPaths::ProjectDir();
    Args->TryGetStringField(TEXT("rootDir"), RootDir);
    RootDir = ResolveWorkspacePath(RootDir);

    FUTAExecutionPolicy Policy;
    if (!Policy.CanReadPath(RootDir))
    {
        return MakeErrorResult(FString::Printf(TEXT("Search access denied for rootDir: %s"), *RootDir));
    }

    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *RootDir, TEXT("*.*"), true, false, false);

    TArray<TSharedPtr<FJsonValue>> MatchValues;
    MatchValues.Reserve(64);

    for (const FString& FilePath : Files)
    {
        if (!Policy.CanReadPath(FilePath))
        {
            continue;
        }

        FString Content;
        if (!FFileHelper::LoadFileToString(Content, *FilePath))
        {
            continue;
        }

        if (Content.Contains(Query, ESearchCase::IgnoreCase))
        {
            MatchValues.Add(MakeShared<FJsonValueString>(FilePath));
            if (MatchValues.Num() >= 100)
            {
                break;
            }
        }
    }

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("query"), Query);
    Json->SetArrayField(TEXT("matches"), MatchValues);

    FUTAToolResult Result;
    Result.bSuccess = true;
    Result.JsonPayload = ToJsonString(Json);
    return Result;
}

FString FUTAWriteFileTool::GetName() const
{
    return TEXT("write_file");
}

FUTAToolResult FUTAWriteFileTool::Execute(const FString& JsonArguments)
{
    TSharedPtr<FJsonObject> Args;
    if (!TryParseArgs(JsonArguments, Args))
    {
        return MakeErrorResult(TEXT("Invalid JSON arguments"));
    }

    bool bApproved = false;
    Args->TryGetBoolField(TEXT("approved"), bApproved);
    if (!bApproved)
    {
        return MakeErrorResult(TEXT("write_file requires explicit approval (approved=true)"));
    }

    FString Path;
    FString Content;
    if (!Args->TryGetStringField(TEXT("path"), Path) || !Args->TryGetStringField(TEXT("content"), Content))
    {
        return MakeErrorResult(TEXT("Missing 'path' or 'content'"));
    }

    const FString FullPath = ResolveWorkspacePath(Path);
    FUTAExecutionPolicy Policy;

    if (!Policy.CanWritePath(FullPath))
    {
        return MakeErrorResult(FString::Printf(TEXT("Write access denied for path: %s"), *FullPath));
    }

    if (!FFileHelper::SaveStringToFile(Content, *FullPath))
    {
        return MakeErrorResult(FString::Printf(TEXT("Failed to write file: %s"), *FullPath));
    }

    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    Json->SetStringField(TEXT("path"), FullPath);
    Json->SetBoolField(TEXT("written"), true);

    FUTAToolResult Result;
    Result.bSuccess = true;
    Result.JsonPayload = ToJsonString(Json);
    return Result;
}
