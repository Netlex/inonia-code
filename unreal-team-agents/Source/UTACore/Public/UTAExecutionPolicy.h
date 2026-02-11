#pragma once

#include "CoreMinimal.h"

class FUTAExecutionPolicy
{
public:
    FUTAExecutionPolicy();

    bool IsPathInsideProject(const FString& AbsolutePath) const;
    bool CanReadPath(const FString& AbsolutePath) const;
    bool CanWritePath(const FString& AbsolutePath) const;

    const FString& GetProjectRoot() const;

private:
    FString ProjectRoot;
    TArray<FString> ReadDenyPrefixes;
    TArray<FString> WriteAllowPrefixes;
};
