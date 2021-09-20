#include "PathNode.h"
#include "DrawDebugHelpers.h"
#include "Math/Color.h"
UPathNode::UPathNode()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPathNode::DrawNode(int i)
{
	return;
	FLinearColor ColorStatus;
	ColorStatus = (FLinearColor::LerpUsingHSV(FColor::Red, FColor::Blue, ((float)i/25.0f)));

	DrawDebugLine(GetWorld(), Position, Position + FVector::UpVector * 100, ColorStatus.ToFColor(true), false, -1.0f, 0, 2.0f);

	if (blocked)
		ColorStatus = FLinearColor::Red;
	FPlane Plane = FPlane(0, 0, 1, Position.Z + 1);
	
	//DrawPlane
	DrawDebugSolidPlane(GetWorld(), Plane, Position, FVector2D(NodeSize / 2 - 5, NodeSize / 2 - 5), ColorStatus.ToFColor(true), false, 0.05f, 0);
	//Draw GridBox
	DrawDebugBox(GetWorld(), Position, FVector(NodeSize * 0.5f - 1, NodeSize * 0.5f - 1, 5.0f), FColor::Black, false, 0.05f, 0, 3.0f);
}