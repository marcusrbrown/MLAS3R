#include "MLAS3REditor.h"
#include "SplineTriggerComponentVisualizer.h"


FSplineTriggerComponentVisualizer::FSplineTriggerComponentVisualizer()
{
	
}

FSplineTriggerComponentVisualizer::~FSplineTriggerComponentVisualizer()
{
	
}

void FSplineTriggerComponentVisualizer::OnRegister()
{
	FComponentVisualizer::OnRegister();
}

void FSplineTriggerComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	//cast the component into the expected component type
	if (const USplineTriggerComponent* splineTriggerComponent = Cast<const USplineTriggerComponent>(Component))
	{
		//get colors for selected and unselected targets
		//This is an editor only uproperty of our targeting component, that way we can change the colors if we can't see them against the background
		//const FLinearColor SelectedColor = TargetingComponent->EditorSelectedColor;
		//const FLinearColor UnselectedColor = TargetingComponent->EditorUnselectedColor;
		
		//const FVector Locaction = TargetingComponent->GetComponentLocaction();
		
		//Iterate over each target drawing a line and dot
		USplineComponent* spline = splineTriggerComponent->Owner->Spline;
		for (int i = 0; i < splineTriggerComponent->Owner->Triggers.Num(); i++)
		{
			FSplineTriggerNode& triggerNode = splineTriggerComponent->Owner->Triggers[i];
			
			FLinearColor color = triggerNode.bActionValue ? FLinearColor(0.0, 1.0, 0.0) : FLinearColor(1.0, 0.0, 0.0);
			FVector position = spline->GetLocationAtTime(triggerNode.Time, ESplineCoordinateSpace::World, true);
			PDI->DrawPoint(position, color, 20.0f, SDPG_Foreground);
		}
	}
}

bool FSplineTriggerComponentVisualizer::VisProxyHandleClick(FLevelEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click)
{
	return false;
}

void FSplineTriggerComponentVisualizer::EndEditing()
{
	FComponentVisualizer::EndEditing();
}

bool FSplineTriggerComponentVisualizer::GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const
{
	return false;
}

bool FSplineTriggerComponentVisualizer::GetCustomInputCoordinateSystem(const FEditorViewportClient* ViewportClient, FMatrix& OutMatrix) const
{
	return false;
}

bool FSplineTriggerComponentVisualizer::HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale)
{
	return false;
}

bool FSplineTriggerComponentVisualizer::HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	return false;
}

TSharedPtr<SWidget> FSplineTriggerComponentVisualizer::GenerateContextMenu() const
{
	return FComponentVisualizer::GenerateContextMenu();
}
