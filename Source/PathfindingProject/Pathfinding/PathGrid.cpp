#include "PathGrid.h"
#include "PathNode.h"


#include "DrawDebugHelpers.h"
#include "Math/Color.h"

APathGrid::APathGrid()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void APathGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugBox(GetWorld(), GetActorLocation(), FVector(AreaLength, AreaWidth, AreaHeight), FColor::Orange, false, -1.0f, 0, 5.0f);
	DrawGrid();
}

void APathGrid::BeginPlay()
{
	Super::BeginPlay();
	AreaWidth = Collums * GridNodeSize;
	AreaLength = Rows * GridNodeSize;
	GenerateGrid();
}

void APathGrid::GenerateGrid()
{
	//X = Vertical
	float Row =  GridNodeSize * (Rows * 0.5f - 0.5f) + GetActorLocation().X;
	//Y = Horizontal
	float Collum = -GridNodeSize * (Collums * 0.5f - 0.5f) + GetActorLocation().Y;

	for(int i = 0; i < Rows; i++)
	{
		for (int j = 0; j < Collums; j++)
		{
			UPathNode* Node = NewObject<UPathNode>();
			Node->Position = FVector(Row - (GridNodeSize * i), Collum + (GridNodeSize * j), GetActorLocation().Z);
			Node->HorizontalValue = i;
			Node->VerticalValue = j;
			Node->NodeSize = GridNodeSize;
			int Test;

			if(i > 0)
			{
				Test = j + Rows * (i - 1);
				if(Test >= 0 && Test < GridBoard.Num())
				{
					Node->North = GridBoard[Test];
					GridBoard[Test]->South = Node;
				}
			}
			if (j > 0)
			{
				Test = (j - 1) + (Rows * i);
				if(Test >= 0 && Test < GridBoard.Num())
				{
					Node->West = GridBoard[Test];
					GridBoard[Test]->East = Node;
				}
			}
			GridBoard.Add(Node);
		}		
	}
	DrawGrid();
}

void APathGrid::SwitchBlocked(int i)
{
	if(i >= 0 && i < GridBoard.Num())
	{
		GridBoard[i]->blocked = !GridBoard[i]->blocked;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Index does not exist"));
	}
}

int APathGrid::ConvertPositionToNodeIndex(FVector Position)
{
	FVector PosDiff = Position - GetActorLocation();
	DrawDebugLine(GetWorld(), Position, Position + FVector::UpVector * 100, FColor::Black, false, -1.0f, 0, 5.0f);

	float Row = (PosDiff.X / GridNodeSize) - (Rows * 0.5f);
	float Collum = (PosDiff.Y / GridNodeSize) + (Collums * 0.5f);
	UE_LOG(LogTemp, Log, TEXT(" Row: %f and Collum: %f"), Row, Collum);
	int iRow = (int)Row;
	int iCollum = (int)Collum;
	int Index = iRow * Collums + iCollum;
	UE_LOG(LogTemp, Log, TEXT(" index at (Row: %i and Collum: %i) is %i"), iRow, iCollum, Index);
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
	if (i >= 0 && i < GridBoard.Num())
	{
		CurrentNode = GridBoard[i];
	}
	else
	{
		CurrentNode = nullptr;
	}
}

void APathGrid::DrawGrid()
{
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + FVector::UpVector * 100, FColor::Cyan, false, -1.0f, 0, 5.0f);
	
	//Collums - Red
	DrawDebugLine(GetWorld(), GridBoard[0]->Position, GridBoard[Rows -1]->Position , FColor::Red, false, -1.0f, 0, 15.0f);
	//Rows - Blue
	DrawDebugLine(GetWorld(), GridBoard[0]->Position, GridBoard[(Collums-1)*Rows]->Position, FColor::Blue, false, -1.0f, 0, 15.0f);
	
	int i = 0;
	for (UPathNode* Node : GridBoard)
	{
		Node->DrawNode(i);
		i++;
		//continue;
		FPlane Plane = FPlane(0, 0, 1, GetActorLocation().Z + 1.0f);
		FLinearColor ColorStatus = FLinearColor::Green;

		if (Node->blocked)
			ColorStatus = FLinearColor::Red;

		if (CurrentNode != nullptr)
		{
			if (CurrentNode == Node)
				ColorStatus = FLinearColor::Blue;
		}
		//ColorStatus = (FLinearColor::LerpUsingHSV(FColor::Green, FColor::Red, PathMultiplier - 1.0f));
		

		//DrawPlane
		DrawDebugSolidPlane(GetWorld(), Plane, Node->Position, FVector2D(GridNodeSize / 2 - 5, GridNodeSize / 2 - 5), ColorStatus.ToFColor(true), false, -1.0f, 0);
		//Draw GridBox
		DrawDebugBox(GetWorld(), Node->Position, FVector(GridNodeSize * 0.5f - 1, GridNodeSize * 0.5f - 1, 5.0f), FColor::Black, false,-1.0f, 0, 3.0f);

	}
}