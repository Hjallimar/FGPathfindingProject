#include "PathGrid.h"
#include "PathNode.h"
#include "Components/LineBatchComponent.h"

#include "DrawDebugHelpers.h"
#include "Math/Color.h"

APathGrid::APathGrid()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	LineBatch = CreateDefaultSubobject<ULineBatchComponent>(TEXT("Linebatch Component"));
	LineBatch->SetupAttachment(RootComponent);
}

void APathGrid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if(GridBoard.Num() == 0)
	{
		PrevRow = Rows;
		PrevCol = Collums;
		AreaWidth = Collums * GridNodeSize;
		AreaLength = Rows * GridNodeSize;
		HalfNodeSize = GridNodeSize * 0.5f;
		GenerateGrid();
		UpdatePathBlocks();
		DrawGrid();
	}
	
	LineBatch->Flush();
}
void APathGrid::BeginPlay()
{
	Super::BeginPlay();
	if(GridBoard.Num()==0)
	{
		PrevRow = Rows;
		PrevCol = Collums;
		AreaWidth = Collums * GridNodeSize;
		AreaLength = Rows * GridNodeSize;
		HalfNodeSize = GridNodeSize * 0.5f;
		UE_LOG(LogTemp, Log, TEXT("Building the thing in beginPlay"));
		GenerateGrid();
		UpdatePathBlocks();
		DrawGrid();
	}
	LineBatch->Flush();
}
bool APathGrid::ShouldTickIfViewportsOnly() const
{
	if (WITH_EDITOR)
		return true;
	return false;
}
void APathGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(PrevRow != Rows || PrevCol != Collums)
	{
		PrevRow = Rows;
		PrevCol = Collums;
		AreaWidth = Collums * GridNodeSize;
		AreaLength = Rows * GridNodeSize;
		HalfNodeSize = GridNodeSize * 0.5f;
		UE_LOG(LogTemp, Log, TEXT("Building the thing in beginPlay"));
		GenerateGrid();
		UpdatePathBlocks();
		DrawGrid();
	}
	else
	{
		if (GridBoard.Num() == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("Building the thing in tick"));
		}
		else
		{
			DrawDebugBox(GetWorld(), GetActorLocation(), FVector(AreaLength / 2, AreaWidth / 2, AreaHeight / 2), FColor::Orange, false, -1.0f, 0, 5.0f);
			TArray<UPathGridBlocker*> BlockersDetected;
			GetComponents(BlockersDetected);
			for (UPathGridBlocker* Block : BlockersDetected)
			{
				AddBlocker(Block);
			}

			UpdatePathBlocks();
			DrawGrid();
		}
	}
}

//Visual
void APathGrid::GenerateGrid()
{
	GridBoard.Empty(0);
	PrevRow = Rows;
	PrevCol = Collums;
	if (Rows < 1 || Collums < 1)
		return;
	float Position_X = (HalfNodeSize * Rows) + GetActorLocation().X;
	float Position_Y = (-HalfNodeSize * Collums) + GetActorLocation().Y;
	FVector BasePos = FVector(Position_X - HalfNodeSize, Position_Y + HalfNodeSize, GetActorLocation().Z);

	for(int x = 0; x < Rows; x++)
	{
		for (int y = 0; y < Collums; y++)
		{
			UPathNode* Node = NewObject<UPathNode>();
			Node->Position = BasePos + FVector(-GridNodeSize * x, GridNodeSize * y, 0.0f);
			Node->NodeSize = GridNodeSize;
			Node->NodeIndex = x * Rows + y;
			int Test;

			//Assign Neighbors
			if(x > 0)
			{
				Test = y + Rows * (x - 1);
				if(Test >= 0 && Test < GridBoard.Num())
				{
					//Assign Up and Down
					Node->AddNeighbour(GridBoard[Test]);
					GridBoard[Test]->AddNeighbour(Node);
					if (y > 0)
					{

						//Assign Diagonal
						Test = y - 1 + Rows * (x - 1);					
						Node->AddDiagonalNeighbour(GridBoard[Test]);
						GridBoard[Test]->AddDiagonalNeighbour(Node);
					}
					if (y < GridBoard.Num() - 1)
					{
						//Assign Diagonal
						Test = y + 1 + Rows * (x - 1);
						Node->AddDiagonalNeighbour(GridBoard[Test]);
						GridBoard[Test]->AddDiagonalNeighbour(Node);
					}
				}
			}
			if (y > 0)
			{
				Test = (y - 1) + (Rows * x);
				if(Test >= 0 && Test < GridBoard.Num())
				{
					Node->AddNeighbour(GridBoard[Test]);
					GridBoard[Test]->AddNeighbour(Node);
				}
			}
			GridBoard.Add(Node);
		}		
	}
}
void APathGrid::DrawGrid()
{
	if(GridBoard.Num() < Rows * Collums)
	{
		return;
	}
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + FVector::UpVector * 100, FColor::Cyan, false, -1, 0, 5.0f);
	
	int i = 0;
	for (UPathNode* Node : GridBoard)
	{
		if(Node == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("node was null and size for the gridboard is %i"), GridBoard.Num());
			return;
		}
		//Node->DrawNode(i);
		//continue;
		i++;
		FPlane Plane = FPlane(0, 0, 1, GetActorLocation().Z + 1.0f);
		FLinearColor ColorStatus = (FLinearColor::LerpUsingHSV(FColor::Green, FColor::Red, ((Node->PathMultiplier - 1) / 3)));

		if (Node->blocked)
			ColorStatus = FLinearColor::Red;

		if (CurrentNode != nullptr)
		{
			if (CurrentNode == Node)
				ColorStatus = FLinearColor::Blue;
		}	

		float size = GridNodeSize * 0.5f;
		DrawDebugSolidPlane(GetWorld(), Plane, Node->Position, FVector2D(size - 5, size - 5), ColorStatus.ToFColor(true), false, -1, 0);
		DrawDebugBox(GetWorld(), Node->Position, FVector(size - 1, size - 1, 5.0f), FColor::Black, false, -1, 0, 3.0f);
	}

	for (UPathGridBlocker* Blocker : Blockers)
	{
		DrawDebugBox(GetWorld(), Blocker->GetComponentLocation(), FVector(Blocker->BlockInfo.Length, Blocker->BlockInfo.Width, Blocker->BlockInfo.Height), FColor::Orange, false, -1, 0, 4.0f);
	}

}
void APathGrid::UpdatePathBlocks()
{
	for(int i = 0; i < Blockers.Num(); i++)
	{
		FBlockerInfo BlockInfo = Blockers[i]->BlockInfo;
		if(BlockInfo.CenterPosition != Blockers[i]->GetComponentLocation())
		{
			for (UPathNode* Node : GridBoard)
			{
				DrawDebugBox(GetWorld(), Blockers[i]->GetComponentLocation(), FVector(BlockInfo.Length, BlockInfo.Width, BlockInfo.Height), FColor::Orange, false, -1, 0, 4.0f);
				if (IsInsideBounderies(Node->Position, Blockers[i]->GetComponentLocation(), FVector(BlockInfo.Length, BlockInfo.Width, BlockInfo.Height)))
				{
					Node->blocked = BlockInfo.Block;
					Node->PathMultiplier = BlockInfo.Multiplier;
				}
				else
				{
					Node->blocked = false;
					Node->PathMultiplier = 1.0f;
				}
			}
			Blockers[i]->BlockInfo.CenterPosition = Blockers[i]->GetComponentLocation();
		}
	}
}

//Functions
void APathGrid::SwitchBlocked(int i)
{
	if(i >= 0 && i < GridBoard.Num())
	{
		GridBoard[i]->blocked = !GridBoard[i]->blocked;
	}
}
void APathGrid::AddBlocker(UPathGridBlocker* Blocker)
{
	Blocker->BlockInfo.CenterPosition = Blocker->GetComponentLocation() - FVector(1,0,0);
	if(!Blockers.Contains(Blocker))
	{
		Blockers.Add(Blocker);
		UE_LOG(LogTemp, Log, TEXT("Added"))
	}
}

//Coverting
int APathGrid::ConvertPositionToNodeIndex(FVector Position)
{
	if (!CheckWorldlocationInGrid(Position))
		return -1;
	FVector PosDiff = Position - GetActorLocation();
	DrawDebugLine(GetWorld(), Position, Position + FVector::UpVector * 100, FColor::Black, false, -1.0f, 0, 5.0f);
	
	float Row = Rows - (PosDiff.X / GridNodeSize) - (Rows * 0.5f);
	float Collum = (PosDiff.Y / GridNodeSize) + (Collums * 0.5f);
	int iRow = (int)Row;
	int iCollum = (int)Collum;
	if (iRow < 0 || iCollum < 0)
		return -1;
	if (iRow >= Rows || iCollum >= Collums)
		return -1;
	int Index = iRow * Collums + iCollum;
	if(Index >= 0 && Index < GridBoard.Num())
	{
		return Index;
	}
	return -1;
}
FVector APathGrid::ConvertNodeIndexToPosition(int index)
{
	if (index >= 0 && index < GridBoard.Num())
		return GridBoard[index]->Position;
	return FVector::ZeroVector;
}
void APathGrid::UpdateCurrentNode(int i)
{
	CurrentNode = (i >= 0 && i < GridBoard.Num()) ? GridBoard[i] : nullptr;
}

//A* path calculation
TArray<int> APathGrid::JustGiveMePath(UPathNode* StartNode, UPathNode* EndNode)
{
	bool randomshit = false;

	if (randomshit)
	{
		//Construct new grid
		TArray<FGridNode> TempGrid = TArray<FGridNode>{};
		for (UPathNode* Node : GridBoard)
		{
			FGridNode NewNode = FGridNode();
			NewNode.GScore = 1000000.0f;
			NewNode.HScore = 1000000.0f;
			NewNode.bBlocker = Node->blocked;
			NewNode.Index = Node->NodeIndex;
			NewNode.Parent = nullptr;
			for (UPathNode* Neighbour : Node->Neighbours)
			{
				NewNode.NeighbourIndex.Add(Neighbour->NodeIndex);
			}
			TempGrid.Add(NewNode);
		}
		//Construction done
		FGridNode* CurrNode = &TempGrid[StartNode->NodeIndex];
		CurrNode->GScore = 0.0f;
		CurrNode->HScore = ICry(StartNode->Position, EndNode->Position);
		TArray<FGridNode*> NotTestedNodes;
		NotTestedNodes.Add(CurrNode);

		while (NotTestedNodes.Num() > 0 && CurrNode != &TempGrid[EndNode->NodeIndex])
		{
			NotTestedNodes.Sort();
			CurrNode = NotTestedNodes[0];
			CurrNode->bChecked = true;
			NotTestedNodes.RemoveAt(0);
			for (int i : CurrNode->NeighbourIndex)
			{
				if (!TempGrid[i].bChecked && !TempGrid[i].bBlocker)
					NotTestedNodes.Add(&TempGrid[i]);

				float PossibleLower = CurrNode->GScore + ICry(GridBoard[CurrNode->Index]->Position, EndNode->Position);
				if (PossibleLower < TempGrid[i].GScore)
				{
					TempGrid[i].Parent = CurrNode;
					TempGrid[i].GScore = PossibleLower;
					TempGrid[i].HScore = TempGrid[i].GScore + ICry(GridBoard[i]->Position, EndNode->Position);
				}
			}
		}

		TArray<int> intPathing = {};
		TArray<FVector> VectPathing = {};
		int newIndex = EndNode->NodeIndex;
		while (TempGrid[newIndex].Parent != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("U have parent"));
			intPathing.Add(newIndex);
			VectPathing.Add(GridBoard[newIndex]->Position);
			newIndex = TempGrid[newIndex].Parent->Index;
		}
		if (VectPathing.Num() > 0)
		{
			DrawDebugLine(GetWorld(), VectPathing[0], VectPathing[0] + FVector::UpVector * 100.0f, FColor::Blue, false, 10.0f, 0, 4.0f);
			float i = 0.0f;
			for (FVector pos : VectPathing)
			{
				FLinearColor Color = FLinearColor::LerpUsingHSV(FLinearColor::Blue, FLinearColor::Red, i / float(VectPathing.Num()));
				i++;
				DrawDebugSphere(GetWorld(), pos, HalfNodeSize, 8, Color.ToFColor(true), false, 10.0f, 0, 4.0f);
			}
			DrawDebugLine(GetWorld(), VectPathing.Last(), VectPathing.Last() + FVector::UpVector * 100.0f, FColor::Red, false, 10.0f, 0, 4.0f);
		}
		return intPathing;
	}
	else
	{
		TArray<FNodeNavigationInfo> ClosedList = {};
		FNodeNavigationInfo NodeNav = FNodeNavigationInfo();
		NodeNav.Node = StartNode;
		FVector Dist = EndNode->Position - StartNode->Position;
		NodeNav.HScore = (Dist.X / GridNodeSize) + (Dist.Y / GridNodeSize) + (Dist.Z / GridNodeSize);
		TArray<FNodeNavigationInfo> OpenList = { NodeNav };

		int counter = 1;
		float StepCost = 10.0f;

		while (OpenList.Num() > 0)
		{
			counter++;
			if (OpenList[0].Node == EndNode)
			{
				NodeNav = OpenList[0];
				UE_LOG(LogTemp, Log, TEXT("End Has been found"));
				break;
			}
			FNodeNavigationInfo Parent = OpenList[0];
			OpenList.Remove(Parent);
			ClosedList.Add(Parent);

			for (UPathNode* Neighbour : Parent.Node->Neighbours)
			{
				FNodeNavigationInfo NeighbourNav = FNodeNavigationInfo();
				NeighbourNav.Node = Neighbour;
				NeighbourNav.Previous = &Parent;
				if (Neighbour->blocked)
				{
					if (OpenList.Contains(NeighbourNav))
					{
						OpenList.Remove(NeighbourNav);
					}
					if (!ClosedList.Contains(NeighbourNav))
					{
						ClosedList.Add(NeighbourNav);
					}
				}

				NeighbourNav.GScore = Parent.GScore + (Neighbour->PathMultiplier * StepCost);
				FVector Heuristic = EndNode->Position - Neighbour->Position;
				NeighbourNav.HScore = (Heuristic.X / GridNodeSize) + (Heuristic.Y / GridNodeSize) + (Heuristic.Z / GridNodeSize);

				if (!ClosedList.Contains(NeighbourNav))
				{
					if (!OpenList.Contains(NeighbourNav))
					{
						OpenList.Add(NeighbourNav);
					}
					else
					{
						int i = 0;
						for (FNodeNavigationInfo Info : OpenList)
						{
							if (Info == NeighbourNav)
								break;

							i++;
						}

						if (OpenList[i].GScore > NeighbourNav.GScore)
						{
							OpenList.RemoveAt(i);
							OpenList.Add(NeighbourNav);
						}
					}
				}
			}
			OpenList.Sort();
			if (OpenList.Num() == 0)
			{
				UE_LOG(LogTemp, Log, TEXT("Didn't find the correct node, Search Made %i times"), counter);
			}
		}

		TArray<int> Index = {};

		UE_LOG(LogTemp, Log, TEXT("Done with search, Node exists: %i, Parent exists: %i, Search Made %i times"), NodeNav.Node, NodeNav.Previous, counter);

		if (OpenList.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("List is bigger than 0"));

			NodeNav = OpenList[0];
		}
		UE_LOG(LogTemp, Log, TEXT("Just Deside to stop work here - 1"));
		while (NodeNav.Previous != nullptr)
		{
			Index.Add(NodeNav.Node->NodeIndex);
			NodeNav = *NodeNav.Previous;
		}
		TArray<FVector> Path = {};
		for (int i : Index)
		{
			Path.Add(GridBoard[i]->Position);
		}
		UE_LOG(LogTemp, Log, TEXT("Just Deside to stop work here - 2"));
		UE_LOG(LogTemp, Log, TEXT("Path has size %i"), Path.Num());

		if (Path.Num() > 0)
		{
			DrawDebugLine(GetWorld(), Path[0], Path[0] + FVector::UpVector * 100.0f, FColor::Blue, false, 10.0f, 0, 4.0f);
			float i = 0.0f;
			for (FVector pos : Path)
			{
				FLinearColor Color = FLinearColor::LerpUsingHSV(FLinearColor::Blue, FLinearColor::Red, i / float(Path.Num()));
				i++;
				DrawDebugSphere(GetWorld(), pos, HalfNodeSize, 8, Color.ToFColor(true), false, 10.0f, 0, 4.0f);
			}
			DrawDebugLine(GetWorld(), Path.Last(), Path.Last() + FVector::UpVector * 100.0f, FColor::Red, false, 10.0f, 0, 4.0f);
		}
		UE_LOG(LogTemp, Log, TEXT("Just Deside to stop work here - 3"));
		return Index;
	}
}
float APathGrid::ICry(FVector a, FVector b)
{
	return FMath::Square((a.X - b.X) * (a.X - b.X) + (a.Y - b.Y) * (a.Y - b.Y));
}

TArray<FVector> APathGrid::CalculatePath(UPathNode* StartNode, UPathNode* EndNode)
{
	TArray<int> intPath = JustGiveMePath(StartNode, EndNode);
	TArray<FVector> Path;
	return Path;
	if(StartNode == nullptr || EndNode == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Start or End is null"))
		Path = {};
		return Path;
	}
	bool OldCalculation = false;
	if (!OldCalculation)
	{
		UE_LOG(LogTemp, Log, TEXT("USING MARCUS CALCULATION"))
		FPathInfo PathInfo = FPathInfo(StartNode, EndNode);
		FNodeNavigationInfo NodeNav = FNodeNavigationInfo();
		NodeNav.Node = StartNode;
		PathInfo.OpenList.Add(NodeNav);
		UE_LOG(LogTemp, Log, TEXT("OpenList has: %i members"), PathInfo.OpenList.Num())
		FNodeNavigationInfo* FinalNode = StartNode->RecursivePathCalculation(&PathInfo);
		if(FinalNode != nullptr)
		{
			while (FinalNode->Previous != nullptr)
			{
				Path.Add(FinalNode->Node->Position);
				FinalNode = FinalNode->Previous;
			}
		}
	}	
	else if(OldCalculation)
	{
		UE_LOG(LogTemp, Log, TEXT("USING OLD CALCULATION"))
		TArray<UPathNode*> ClosedList;
		TArray<UPathNode*> OpenList;
		if (StartNode == nullptr || EndNode == nullptr)
			return TArray<FVector> {};
		UPathNode* CurrentItterNode = StartNode;
		float GPath = 0.0;
		FNodeInfo NewParent = FNodeInfo();
		int SafetyNet = 0;
		while (CurrentItterNode != EndNode || SafetyNet > 25)
		{
			SafetyNet++;
			FNodeInfo BestFit = FNodeInfo();
			if (NewParent.ParentNode != nullptr)
			{
				CurrentItterNode->Parent = NewParent;
				BestFit.ParentNode = NewParent.Node;
			}
			else
			{
				CurrentItterNode->Parent = FNodeInfo();
				CurrentItterNode->Parent.Node = CurrentItterNode;
				BestFit.ParentNode = CurrentItterNode;
			}

			CurrentItterNode->GatherNeighbours(OpenList, ClosedList); //Place neighbours.4

			for (UPathNode* Node : CurrentItterNode->Neighbours)
			{
				if (Node != nullptr)
					continue;

				if (!ClosedList.Contains(Node) && !Node->blocked)
				{
					CurrentItterNode->CheckNodes(&BestFit, GPath, Node, EndNode);
				}
			}
			CurrentItterNode = BestFit.Node;
		}


		//if(StartNode == EndNode)
		//{
		//	Path = { StartNode->Position };
		//	return Path;
		//}
		//else
		//{
		//	FVector Distance = EndNode->Position - StartNode->Position;
		//	float FDistance = FMath::Pow(Distance.X, 2.0f) + FMath::Pow(Distance.Y, 2.0f) + FMath::Pow(Distance.Z, 2.0f);
		//	TArray<UPathNode*> ClosedList;
		//	TArray<UPathNode*> OpenList;
		//	Path = StartNode->CalculatePath(nullptr, EndNode, 0.0f, OpenList, ClosedList);

		//}
		Path = CurrentNode->GeneratePath();
	}

	DrawDebugLine(GetWorld(), Path[0], Path[0] + FVector::UpVector * 100.0f, FColor::Blue, false, 10.0f, 0, 4.0f);
	float i = 0.0f;
	for (FVector pos : Path)
	{
		FLinearColor Color = FLinearColor::LerpUsingHSV(FLinearColor::Blue, FLinearColor::Red, i / float(Path.Num()));
		i++;
		DrawDebugSphere(GetWorld(), pos, HalfNodeSize, 8, Color.ToFColor(true), false, 10.0f, 0, 4.0f);
	}
	DrawDebugLine(GetWorld(), Path.Last(), Path.Last() + FVector::UpVector * 100.0f, FColor::Red, false, 10.0f, 0, 4.0f);

	UE_LOG(LogTemp, Log, TEXT("Total of %i nodes were counted"), Path.Num());
	return Path;
}
TArray<FVector> APathGrid::CalculatePath(FVector StartPos, FVector EndPos)
{
	if (!CheckWorldlocationInGrid(StartPos) || !CheckWorldlocationInGrid(EndPos))
		return TArray<FVector> {};

	int StartIndex = ConvertPositionToNodeIndex(StartPos);
	int EndIndex = ConvertPositionToNodeIndex(EndPos);
	return CalculatePath(GridBoard[StartIndex], GridBoard[EndIndex]);
}
TArray<FVector> APathGrid::CalculatePath(int StartIndex, int EndIndex)
{
	if (StartIndex < 0 || EndIndex < 0 || StartIndex >= GridBoard.Num() || EndIndex >= GridBoard.Num())
		return TArray<FVector> {};
	return CalculatePath(GridBoard[StartIndex], GridBoard[EndIndex]);
}

//Math location stuff
bool APathGrid::IsInsideBounderies(FVector position, FVector Center, FVector Bounderies)
{
	FVector checker = Center + Bounderies;
	if (position.X > checker.X || position.Y > checker.Y || position.Z > checker.Z)
		return false;
	checker = Center - Bounderies;
	if (position.X < checker.X || position.Y < checker.Y || position.Z < checker.Z)
		return false;
	return true;
}
bool APathGrid::CheckWorldlocationInGrid(const FVector& WorldLocation) const
{
	FVector RelativeVector = GetActorTransform().InverseTransformPositionNoScale(WorldLocation);
	if (RelativeVector.X < -HalfNodeSize * Rows)
		return false;
	else if (RelativeVector.X > HalfNodeSize * Rows)
		return false;
	else if (RelativeVector.Y < -HalfNodeSize * Collums)
		return false;
	else if (RelativeVector.Y > HalfNodeSize * Collums)
		return false;
	return true;
}