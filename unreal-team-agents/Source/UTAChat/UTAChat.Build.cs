using UnrealBuildTool;

public class UTAChat : ModuleRules
{
    public UTAChat(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UTACore",
            "Json"
        });
    }
}
