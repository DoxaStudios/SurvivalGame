// Some copyright should be here...

using UnrealBuildTool;

public class AutoTerrainTexturer : ModuleRules
{
	public AutoTerrainTexturer(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				"AutoTerrainTexturer/Public"
				
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"AutoTerrainTexturer/Private",
				
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject", "Engine", "Slate", "SlateCore"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
