#include "PathNode.h"
#include "DrawDebugHelpers.h"
#include "Math/Color.h"
UPathNode::UPathNode()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPathNode::DrawNode()
{
	FPlane Plane = FPlane(0, 0, 1, Position.Z + 1);
	FLinearColor ColorStatus;
	ColorStatus = (FLinearColor::LerpUsingHSV(FColor::Green, FColor::Red, PathMultiplier - 1.0f));
	if (blocked)
		ColorStatus = FLinearColor::Red;
	
	//DrawPlane
	DrawDebugSolidPlane(GetWorld(), Plane, Position, FVector2D(NodeSize / 2 - 5, NodeSize / 2 - 5), ColorStatus.ToFColor(true), false, 0.05f, 0);
	//Draw GridBox
	DrawDebugBox(GetWorld(), Position, FVector(NodeSize * 0.5f - 1, NodeSize * 0.5f - 1, 5.0f), FColor::Black, false, 0.05f, 0, 3.0f);
}