using UnrealBuildTool;

public class UTABlueprint : ModuleRules
{
    public UTABlueprint(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Kismet",
            "BlueprintGraph",
            "UnrealEd",
            "UTACore"
        });
    }
}
