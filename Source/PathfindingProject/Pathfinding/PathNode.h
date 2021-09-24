#pragma once

#include "Components/SceneComponent.h"
#include "PathNode.generated.h"

USTRUCT()
struct FNodeInfo
{
	GENERATED_BODY()
public:
	UPathNode* Node = nullptr;
	float HScore = 0.0f;
	float GScore = 0.0f;
	float FScore = 0.0f;
	FNodeInfo()
	{
		Node = nullptr;
		HScore = 0.0f;
		GScore = 0.0f;
		FScore = 0.0f;
	}

};
UCLASS()
class UPathNode : public USceneComponent
{
	GENERATED_BODY()
public:
	UPathNode();
	UPROPERTY(VisibleAnywhere)
	FVector Position;
	UPROPERTY(VisibleAnywhere)
	float NodeSize = 0;
	UPROPERTY(VisibleAnywhere)
	bool blocked = false;
	UPROPERTY(VisibleAnywhere)
	float PathMultiplier = 1.0f;

	void DrawNode(int i);
	TArray<FVector> CalculatePath(UPathNode* Parent, UPathNode* Goal, float GPath, TArray<UPathNode*> OpenList, TArray<UPathNode*> ClosedList);
	UPROPERTY()
	UPathNode* Up; 
	UPROPERTY()
	UPathNode* Left; 
	UPROPERTY()
	UPathNode* Down;
	UPROPERTY()
	UPathNode* Right; 
	// X --- Diagonal, adding if I find time
	UPROPERTY()
	UPathNode* UpLeft; 
	UPROPERTY()
	UPathNode* UpRight; 
	UPROPERTY()
	UPathNode* DownRight; 
	UPROPERTY()
	UPathNode* DownLeft;
private:
	void CheckNodes(FNodeInfo* BestFit, float GPath, UPathNode* CheckNode, UPathNode* Goal);
	void GatherNeighbours(TArray<UPathNode*> OpenList, TArray<UPathNode*> ClosedList);
};