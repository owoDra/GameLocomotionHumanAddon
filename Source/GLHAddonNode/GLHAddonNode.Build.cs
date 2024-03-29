// Copyright (C) 2024 owoDra

using UnrealBuildTool;

public class GLHAddonNode : ModuleRules
{
	public GLHAddonNode(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                ModuleDirectory,
                ModuleDirectory + "/GLHAddonNode",
            }
        );

        PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"AnimationModifiers", 
				"AnimationBlueprintLibrary",
                "GLExt",
                "GLHAddon",
            }
		);

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new[]
				{
					"AnimGraph",
					"AnimGraphRuntime",
					"BlueprintGraph"
				}
			);
		}
	}
}