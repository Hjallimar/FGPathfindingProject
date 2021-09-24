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
			int Test;

			//Assign Neighbors
			if(x > 0)
			{
				Test = y + Rows * (x - 1);
				if(Test >= 0 && Test < GridBoard.Num())
				{
					//Assign Up & Down
					Node->Up = GridBoard[Test];
					GridBoard[Test]->Down = Node;
					if (y > 0)
					{
						//Assign Diagonal
						Test = y - 1 + Rows * (x - 1);
						Node->UpLeft = GridBoard[Test];
						GridBoard[Test]->DownRight = Node;
					}
					if (y < GridBoard.Num() - 1)
					{
						//Assign Diagonal
						Test = y + 1 + Rows * (x - 1);
						Node->UpRight = GridBoard[Test]; 
						GridBoard[Test]->DownLeft = Node;
					}
				}
			}
			if (y > 0)
			{
				Test = (y - 1) + (Rows * x);
				if(Test >= 0 && Test < GridBoard.Num())
				{
					//Assign Right & left
					Node->Left = GridBoard[Test];
					GridBoard[Test]->Right = Node;
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
TArray<FVector> APathGrid::CalculatePath(UPathNode* StartNode, UPathNode* EndNode)
{
	if(StartNode == EndNode)
	{
		TArray<FVector> Path = { StartNode->Position };
		return Path;
	}

	FVector Distance = EndNode->Position - StartNode->Position;
	float FDistance = FMath::Pow(Distance.X, 2.0f) + FMath::Pow(Distance.Y, 2.0f) + FMath::Pow(Distance.Z, 2.0f);
	TArray<UPathNode*> ClosedList;
	TArray<UPathNode*> OpenList;
	TArray<FVector> Path = StartNode->CalculatePath(nullptr, EndNode, 0.0f, OpenList, ClosedList);
	FLinearColor Color; 
	float i = 0.0f;
	DrawDebugLine(GetWorld(), Path[0], Path[0] + FVector::UpVector * 100.0f, FColor::Blue, false, 10.0f, 0, 4.0f);

	for (FVector pos : Path)
	{
		Color = FLinearColor::LerpUsingHSV(FLinearColor::Blue, FLinearColor::Red, i / float(Path.Num()));
		i++;
		DrawDebugSphere(GetWorld(), pos, HalfNodeSize, 8, Color.ToFColor(true), false, 10.0f, 0, 4.0f);
	}

	DrawDebugLine(GetWorld(), Path.Last(), Path.Last() + FVector::UpVector * 100.0f, FColor::Red, false, 10.0f, 0, 4.0f);

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