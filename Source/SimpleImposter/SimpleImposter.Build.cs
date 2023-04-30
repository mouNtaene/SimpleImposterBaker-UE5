// Some copyright should be here...

using UnrealBuildTool;
using System.IO;
using System;


public class SimpleImposter : ModuleRules
{
    
    public SimpleImposter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"OpenCVHelper",
                "OpenCV",
                "GeometryCore",
                "GeometryFramework",
                "GeometryScriptingCore",
                "DynamicMesh",
				// ... add other public dependencies that you statically link with here ...
			}
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		

			/*if (Target.Platform == UnrealTargetPlatform.Win64)
			{
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "ThirdParty", "OpenCVHookLib/x64/Release/OpenCVHookLib.lib"));
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "ThirdParty", "OpenCVHookLib/include"));
			};*/
         
    }
}
