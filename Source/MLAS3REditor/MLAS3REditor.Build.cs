// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class MLAS3REditor : ModuleRules
{
	public MLAS3REditor(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "MLAS3R" });

		PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
	}
}
