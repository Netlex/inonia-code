#include "UTAExecutionPolicy.h"

#include "Misc/Paths.h"

namespace
{
FString NormalizeAbsolute(const FString& Path)
{
    FString Result = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeDirectoryName(Result);
    return Result;
}

bool StartsWithAny(const FString& Value, const TArray<FString>& Prefixes)
{
    for (const FString& Prefix : Prefixes)
    {
        if (Value.StartsWith(Prefix))
        {
            return true;
        }
    }

    return false;
}
} // namespace

FUTAExecutionPolicy::FUTAExecutionPolicy()
{
    ProjectRoot = NormalizeAbsolute(FPaths::ProjectDir());

    ReadDenyPrefixes = {
        NormalizeAbsolute(FPaths::Combine(ProjectRoot, TEXT("Saved"))),
    };

    WriteAllowPrefixes = {
        NormalizeAbsolute(FPaths::Combine(ProjectRoot, TEXT("Source"))),
        NormalizeAbsolute(FPaths::Combine(ProjectRoot, TEXT("Plugins"))),
        NormalizeAbsolute(FPaths::Combine(ProjectRoot, TEXT("Content"))),
    };
}

bool FUTAExecutionPolicy::IsPathInsideProject(const FString& AbsolutePath) const
{
    const FString Normalized = NormalizeAbsolute(AbsolutePath);
    return Normalized.StartsWith(ProjectRoot);
}

bool FUTAExecutionPolicy::CanReadPath(const FString& AbsolutePath) const
{
    const FString Normalized = NormalizeAbsolute(AbsolutePath);
    return IsPathInsideProject(Normalized) && !StartsWithAny(Normalized, ReadDenyPrefixes);
}

bool FUTAExecutionPolicy::CanWritePath(const FString& AbsolutePath) const
{
    const FString Normalized = NormalizeAbsolute(AbsolutePath);
    return IsPathInsideProject(Normalized) && StartsWithAny(Normalized, WriteAllowPrefixes);
}

const FString& FUTAExecutionPolicy::GetProjectRoot() const
{
    return ProjectRoot;
}
