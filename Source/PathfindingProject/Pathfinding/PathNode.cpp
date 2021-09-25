#include "PathNode.h"
#include "DrawDebugHelpers.h"
#include "Math/Color.h"
UPathNode::UPathNode()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPathNode::AddNeighbour(UPathNode* Node)
{
	Neighbours.Add(Node);
}
void UPathNode::AddDiagonalNeighbour(UPathNode* Node)
{
	DiagonalNeighbours.Add(Node);
}
FNodeInfo UPathNode::GetParent()
{
	return Parent;
}
void UPathNode::SetParent(UPathNode* NewParent, FNodeScore NewParentScore)
{
	Parent.ParentNode = NewParent;
	Parent.ParentScore = NewParentScore;
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

//TESTING-------------------------------
FNodeNavigationInfo* UPathNode::RecursivePathCalculation(FPathInfo* PathInfo)
{
	return nullptr;
	/*UE_LOG(LogTemp, Log, TEXT("DOING Recursive"))
	if (PathInfo->OpenList.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Eeee wtf, list is empty..."))
			return nullptr;
	}
	if (this == PathInfo->GoalNode)
		return &PathInfo->OpenList[0];

	UE_LOG(LogTemp, Log, TEXT("TestLog 1"))
	FNodeNavigationInfo NodeNav = PathInfo->OpenList[0];
	PathInfo->ClosedList.Add(NodeNav);
	UE_LOG(LogTemp, Log, TEXT("TestLog 2"))
	for(UPathNode* Neighbour : Neighbours)
	{
		FNodeNavigationInfo NeighbourNav = FNodeNavigationInfo();
		NeighbourNav.Node = Neighbour;
		NeighbourNav.Previous = &NodeNav;
		if(Neighbour->blocked)
		{
			if(!PathInfo->ClosedList.Contains(NeighbourNav))
			{
				PathInfo->ClosedList.Add(NeighbourNav);
			}
			continue;
		}

		NeighbourNav.GScore = NodeNav.GScore + (PathMultiplier * StepCost);
		FVector Heuristic = PathInfo->GoalNode->Position - Position;
		NeighbourNav.HScore = (Heuristic.X / NodeSize) + (Heuristic.Y / NodeSize) + (Heuristic.Z / NodeSize);
		if(PathInfo->ClosedList.Contains(NeighbourNav))
		{
			if(PathInfo->OpenList.Contains(NeighbourNav))
			{
				int i = 0;
				for(FNodeNavigationInfo Info : PathInfo->OpenList)
				{
					if(Info == NeighbourNav)
					{
						break;
					}
					i++;
				}

				if(PathInfo->OpenList[i].GScore > NeighbourNav.GScore)
				{
					PathInfo->OpenList.RemoveAt(i);
					PathInfo->OpenList.Add(NeighbourNav);
				}
			}
			else
			{
				PathInfo->OpenList.Add(NeighbourNav);
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("TestLog 3"))
	if (PathInfo->OpenList.Num() == 1)
	{
		if(&NodeNav == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("Place 0 is nullptr"))
			return &NodeNav;
		}
		UE_LOG(LogTemp, Log, TEXT("TestLog 4.1....: %i"), PathInfo->OpenList.Num())
		return &PathInfo->OpenList[0];
	}
	else
	{
		PathInfo->OpenList.Remove(NodeNav);
		PathInfo->OpenList.Sort();
		UE_LOG(LogTemp, Log, TEXT("TestLog 4.2"))
		return PathInfo->OpenList[0].Node->RecursivePathCalculation(PathInfo);
	}*/
}
TArray<FVector> UPathNode::BuildPathFromInfo(FNodeNavigationInfo* Start)
{
	TArray<FVector> Path = {};
	Path.Add(Start->Node->Position);
	if(Start->Previous != nullptr)
	{
		BuildPathFromInfo(Start->Previous);
	}
	return Path;
}
//TESTING-------------------------------OVER

TArray<FVector> UPathNode::CalculatePath(FNodeInfo* NewParent, UPathNode* Goal, float GPath, TArray<UPathNode*> OpenList, TArray<UPathNode*> ClosedList)
{
	//UPathNode* End = CalculatePathToEnd(NewParent, Goal, GPath, OpenList, ClosedList);
	//return GeneratePath(End);

	TArray<FVector> Path;
	FNodeInfo BestFit = FNodeInfo();
	if(NewParent != nullptr)
	{
		Parent = *NewParent;
		BestFit.ParentNode = NewParent->Node;
	}
	else
	{
		Parent = FNodeInfo();
		Parent.Node = this;
		BestFit.ParentNode = this;
	}

	Path.Add(Position);
	GatherNeighbours(OpenList, ClosedList); //Place neighbours.4

	for (UPathNode* Node : Neighbours)
	{
		if (Node != nullptr)
			continue;
		
		if(!ClosedList.Contains(Node) && !Node->blocked)
		{
			CheckNodes(&BestFit, GPath, Node, Goal);
		}
	}

	if(BestFit.Node != nullptr && BestFit.Node == Goal)
	{
		Path.Add(BestFit.Node->Position);
	}
	else if(BestFit.Node != nullptr)
	{
		Path.Append(BestFit.Node->CalculatePath(&BestFit, Goal, BestFit.NodeScore.GScore, OpenList, ClosedList));
	}
	return Path;
}

UPathNode* UPathNode::CalculatePathToEnd(FNodeInfo* NewParent, UPathNode* Goal, float GPath, TArray<UPathNode*> OpenList, TArray<UPathNode*> ClosedList)
{
	FNodeInfo BestFit = FNodeInfo();
	if (NewParent != nullptr)
	{
		Parent = *NewParent;
		BestFit.ParentNode = NewParent->Node;
	}
	else
	{
		Parent = FNodeInfo();
		Parent.Node = this;
		BestFit.ParentNode = this;
	}

	GatherNeighbours(OpenList, ClosedList); //Place neighbours.4

	for (UPathNode* Node : Neighbours)
	{
		if (Node != nullptr)
			continue;

		if (!ClosedList.Contains(Node) && !Node->blocked)
		{
			CheckNodes(&BestFit, GPath, Node, Goal);
		}
	}

	if (BestFit.Node != nullptr && BestFit.Node == Goal)
	{
		return BestFit.Node;
	}
	else if (BestFit.Node != nullptr)
	{
		return BestFit.Node->CalculatePathToEnd(&BestFit, Goal, BestFit.NodeScore.GScore, OpenList, ClosedList);
	}
	return this;
}

TArray<FVector> UPathNode::GeneratePath()
{
	TArray<FVector> Path = {};
	UPathNode* CurrentNode = this;
	while (CurrentNode->GetParent().ParentNode != nullptr)
	{
		Path.Add(CurrentNode->Position);
		CurrentNode = CurrentNode->GetParent().ParentNode;
	}
	return Path;
}

void UPathNode::CheckNodes(FNodeInfo* BestFit, float GPath, UPathNode* CheckNode, UPathNode* Goal)
{
	FVector NewH = Goal->Position - CheckNode->Position;
	if (BestFit->Node == nullptr)
	{
		BestFit->Node = CheckNode;
		BestFit->NodeScore.HScore = (NewH.X / NodeSize) + (NewH.Y / NodeSize);
		BestFit->NodeScore.GScore = GPath + BestFit->Node->PathMultiplier;
		BestFit->NodeScore.FScore = BestFit->NodeScore.HScore + BestFit->NodeScore.GScore;
	}
	else 
	{
		float fNewH = (NewH.X / NodeSize) + (NewH.Y / NodeSize);
		float fNewG = GPath + CheckNode->PathMultiplier;
		float fNewF = fNewH + fNewG;
		if (fNewF < BestFit->NodeScore.FScore)
		{
			BestFit->Node = CheckNode;
			BestFit->NodeScore.HScore = fNewH;
			BestFit->NodeScore.GScore = fNewG;
			BestFit->NodeScore.FScore = fNewF;
		}
	}
}
bool UPathNode::ChangeParents(FNodeScore OldParent, FNodeScore NewParent)
{
	if (NewParent.GScore < OldParent.GScore)
		return true;
	return false;
}

void UPathNode::GatherNeighbours(TArray<UPathNode*> OpenList, TArray<UPathNode*> ClosedList)
{
	for(UPathNode* Node : Neighbours)
	{
		if (Node == nullptr)
		{
			continue;
		}
		if (!ClosedList.Contains(Node) && Node->blocked)
		{
			ClosedList.Add(Node);
		}
		else if (OpenList.Contains(Node))
		{
			if (Node->GetParent().ParentNode != nullptr)
			{
				if (ChangeParents(Node->GetParent().ParentScore, Parent.ParentScore))
				{
					Node->SetParent(this, Parent.ParentScore);
				}
			}
		}
		else if (!ClosedList.Contains(Node) && !OpenList.Contains(Node))
		{
			OpenList.Add(Node);
		}
	}
}