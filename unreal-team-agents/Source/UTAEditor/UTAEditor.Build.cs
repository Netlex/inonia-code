using UnrealBuildTool;

public class UTAEditor : ModuleRules
{
    public UTAEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "UnrealEd",
            "LevelEditor",
            "WorkspaceMenuStructure",
            "ToolMenus",
            "UTACore",
            "UTAChat",
            "UTAUI",
            "UTABlueprint"
        });
    }
}
