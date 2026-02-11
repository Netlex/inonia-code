#include "UTAFileTools.h"

#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"

namespace
{
bool TryParseArgs(const FString& JsonArguments, TSharedPtr<FJsonObject>& OutJson)
{
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonArguments);
    return FJsonSerializer::Deserialize(Reader, OutJson) && OutJson.IsValid();
}

FString MakeErrorJson(const FString& Message)
{
    return FString::Printf(TEXT("{\"error\":\"%s\"}"), *Message.ReplaceCharWithEscapedChar());
}

FString ResolveWorkspacePath(const FString& RelativeOrAbsolutePath)
{
    if (FPaths::IsRelative(RelativeOrAbsolutePath))
    {
        return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), RelativeOrAbsolutePath);
    }

    return FPaths::ConvertRelativePathToFull(RelativeOrAbsolutePath);
}
} // namespace

FString FUTAReadFileTool::GetName() const
{
    return TEXT("read_file");
}

FUTAToolResult FUTAReadFileTool::Execute(const FString& JsonArguments)
{
    FUTAToolResult Result;

    TSharedPtr<FJsonObject> Args;
    if (!TryParseArgs(JsonArguments, Args))
    {
        Result.ErrorMessage = TEXT("Invalid JSON arguments");
        Result.JsonPayload = MakeErrorJson(Result.ErrorMessage);
        return Result;
    }

    FString Path;
    if (!Args->TryGetStringField(TEXT("path"), Path))
    {
        Result.ErrorMessage = TEXT("Missing 'path'");
        Result.JsonPayload = MakeErrorJson(Result.ErrorMessage);
        return Result;
    }

    const FString FullPath = ResolveWorkspacePath(Path);
    FString Content;
    if (!FFileHelper::LoadFileToString(Content, *FullPath))
    {
        Result.ErrorMessage = FString::Printf(TEXT("Failed to read file: %s"), *FullPath);
        Result.JsonPayload = MakeErrorJson(Result.ErrorMessage);
        return Result;
    }

    Result.bSuccess = true;
    Result.JsonPayload = FString::Printf(
        TEXT("{\"path\":\"%s\",\"content\":\"%s\"}"),
        *FullPath.ReplaceCharWithEscapedChar(),
        *Content.ReplaceCharWithEscapedChar());

    return Result;
}

FString FUTASearchFilesTool::GetName() const
{
    return TEXT("search_files");
}

FUTAToolResult FUTASearchFilesTool::Execute(const FString& JsonArguments)
{
    FUTAToolResult Result;

    TSharedPtr<FJsonObject> Args;
    if (!TryParseArgs(JsonArguments, Args))
    {
        Result.ErrorMessage = TEXT("Invalid JSON arguments");
        Result.JsonPayload = MakeErrorJson(Result.ErrorMessage);
        return Result;
    }

    FString Query;
    if (!Args->TryGetStringField(TEXT("query"), Query) || Query.IsEmpty())
    {
        Result.ErrorMessage = TEXT("Missing or empty 'query'");
        Result.JsonPayload = MakeErrorJson(Result.ErrorMessage);
        return Result;
    }

    FString RootDir = FPaths::ProjectDir();
    Args->TryGetStringField(TEXT("rootDir"), RootDir);
    RootDir = ResolveWorkspacePath(RootDir);

    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *RootDir, TEXT("*.*"), true, false, false);

    TArray<FString> Matches;
    Matches.Reserve(64);

    for (const FString& FilePath : Files)
    {
        FString Content;
        if (!FFileHelper::LoadFileToString(Content, *FilePath))
        {
            continue;
        }

        if (Content.Contains(Query, ESearchCase::IgnoreCase))
        {
            Matches.Add(FilePath);
            if (Matches.Num() >= 100)
            {
                break;
            }
        }
    }

    FString Joined;
    for (int32 Index = 0; Index < Matches.Num(); ++Index)
    {
        Joined += FString::Printf(TEXT("\"%s\""), *Matches[Index].ReplaceCharWithEscapedChar());
        if (Index + 1 < Matches.Num())
        {
            Joined += TEXT(",");
        }
    }

    Result.bSuccess = true;
    Result.JsonPayload = FString::Printf(TEXT("{\"query\":\"%s\",\"matches\":[%s]}"), *Query.ReplaceCharWithEscapedChar(), *Joined);
    return Result;
}

FString FUTAWriteFileTool::GetName() const
{
    return TEXT("write_file");
}

FUTAToolResult FUTAWriteFileTool::Execute(const FString& JsonArguments)
{
    FUTAToolResult Result;

    TSharedPtr<FJsonObject> Args;
    if (!TryParseArgs(JsonArguments, Args))
    {
        Result.ErrorMessage = TEXT("Invalid JSON arguments");
        Result.JsonPayload = MakeErrorJson(Result.ErrorMessage);
        return Result;
    }

    bool bApproved = false;
    Args->TryGetBoolField(TEXT("approved"), bApproved);
    if (!bApproved)
    {
        Result.ErrorMessage = TEXT("write_file requires explicit approval (approved=true)");
        Result.JsonPayload = MakeErrorJson(Result.ErrorMessage);
        return Result;
    }

    FString Path;
    FString Content;
    if (!Args->TryGetStringField(TEXT("path"), Path) || !Args->TryGetStringField(TEXT("content"), Content))
    {
        Result.ErrorMessage = TEXT("Missing 'path' or 'content'");
        Result.JsonPayload = MakeErrorJson(Result.ErrorMessage);
        return Result;
    }

    const FString FullPath = ResolveWorkspacePath(Path);
    if (!FFileHelper::SaveStringToFile(Content, *FullPath))
    {
        Result.ErrorMessage = FString::Printf(TEXT("Failed to write file: %s"), *FullPath);
        Result.JsonPayload = MakeErrorJson(Result.ErrorMessage);
        return Result;
    }

    Result.bSuccess = true;
    Result.JsonPayload = FString::Printf(TEXT("{\"path\":\"%s\",\"written\":true}"), *FullPath.ReplaceCharWithEscapedChar());
    return Result;
}
