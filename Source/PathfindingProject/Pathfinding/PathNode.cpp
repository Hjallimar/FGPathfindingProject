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

	DrawDebugLine(GetWorld(), Position, Position + FVector::UpVector * 100, ColorStatus.ToFColor(true), false, -1, 0, 2.0f);

	if (blocked)
		ColorStatus = FLinearColor::Red;
	FPlane Plane = FPlane(0, 0, 1, Position.Z + 1);
	
	//DrawPlane
	DrawDebugSolidPlane(GetWorld(), Plane, Position, FVector2D(NodeSize / 2 - 5, NodeSize / 2 - 5), ColorStatus.ToFColor(true), false, -1, 0);
	//Draw GridBox
	DrawDebugBox(GetWorld(), Position, FVector(NodeSize * 0.5f - 1, NodeSize * 0.5f - 1, 5.0f), FColor::Black, false, -1, 0, 3.0f);
}

TArray<FVector> UPathNode::CalculatePath(UPathNode* Parent, UPathNode* Goal, float GPath, TArray<UPathNode*> OpenList, TArray<UPathNode*> ClosedList)
{
	TArray<FVector> Path;
	FNodeInfo BestFit = FNodeInfo();
	Path.Add(Position);
	GatherNeighbours(OpenList, ClosedList);
	if(OpenList.Contains(this))
	{
		OpenList.Remove(this);
	}
	ClosedList.Add(this);

	if(Up != nullptr && !ClosedList.Contains(Up) && !Up->blocked)
		CheckNodes(&BestFit, GPath, Up, Goal);
	
	if (Down != nullptr && !ClosedList.Contains(Down) && !Down->blocked)
		CheckNodes(&BestFit, GPath, Down, Goal);
	
	if (Left != nullptr && !ClosedList.Contains(Left) && !Left->blocked)
		CheckNodes(&BestFit, GPath, Left, Goal);
	
	if (Right != nullptr && !ClosedList.Contains(Right) && !Right->blocked)
		CheckNodes(&BestFit, GPath, Right, Goal);

	if(BestFit.Node != nullptr && BestFit.Node == Goal)
	{
		Path.Add(BestFit.Node->Position);
	}
	else if(BestFit.Node != nullptr)
	{
		Path.Append(BestFit.Node->CalculatePath(this, Goal, BestFit.GScore, OpenList, ClosedList));
	}
	return Path;
}

void UPathNode::CheckNodes(FNodeInfo* BestFit, float GPath, UPathNode* CheckNode, UPathNode* Goal)
{
	FVector NewH = Goal->Position - CheckNode->Position;
	if (BestFit->Node == nullptr)
	{
		BestFit->Node = CheckNode;
		BestFit->HScore = (NewH.X / NodeSize) + (NewH.Y / NodeSize);
		BestFit->GScore = GPath + BestFit->Node->PathMultiplier;
		BestFit->FScore = BestFit->HScore + BestFit->GScore;
	}
	else 
	{
		float fNewH = (NewH.X / NodeSize) + (NewH.Y / NodeSize);
		float fNewG = GPath + CheckNode->PathMultiplier;
		float fNewF = fNewH + fNewG;
		if (fNewF < BestFit->FScore)
		{
			BestFit->Node = CheckNode;
			BestFit->HScore = fNewH;
			BestFit->GScore = fNewG;
			BestFit->FScore = fNewF;
		}
	}
}

void UPathNode::GatherNeighbours(TArray<UPathNode*> OpenList, TArray<UPathNode*> ClosedList)
{
	if (!OpenList.Contains(Up) && !ClosedList.Contains(Up))
		OpenList.Add(Up);
	if (!OpenList.Contains(Down) && !ClosedList.Contains(Down))
		OpenList.Add(Down);
	if (!OpenList.Contains(Left) && !ClosedList.Contains(Left))
		OpenList.Add(Left);
	if (!OpenList.Contains(Right) && !ClosedList.Contains(Right))
		OpenList.Add(Right);
}