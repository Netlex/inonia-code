using UnrealBuildTool;

public class UTAUI : ModuleRules
{
    public UTAUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "EditorSubsystem",
            "ToolMenus",
            "UTACore",
            "UTAChat"
        });
    }
}
