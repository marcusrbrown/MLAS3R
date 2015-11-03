#include "MLAS3REditor.h"
#include "SplineTriggerComponentVisualizer.h"
 
IMPLEMENT_GAME_MODULE(FMLAS3REditorModule, MLAS3REditor);

DEFINE_LOG_CATEGORY(MLAS3REditor)
 
#define LOCTEXT_NAMESPACE "MyGameEditor"
 
void FMLAS3REditorModule::StartupModule()
{
	UE_LOG(MLAS3REditor, Warning, TEXT("MLAS3REditor: Log Started"));
	
	if (GUnrealEd != NULL)
	{
		TSharedPtr<FComponentVisualizer> Visualizer = MakeShareable(new FSplineTriggerComponentVisualizer());
		if (Visualizer.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(USplineTriggerComponent::StaticClass()->GetFName(), Visualizer);
			Visualizer->OnRegister();
		}
	}
}

void FMLAS3REditorModule::ShutdownModule()
{
	UE_LOG(MLAS3REditor, Warning, TEXT("MLAS3REditor: Log Ended"));
	
	if (GUnrealEd != NULL)
	{
		GUnrealEd->UnregisterComponentVisualizer(USplineTriggerComponent::StaticClass()->GetFName());
	}
}
 
#undef LOCTEXT_NAMESPACE
