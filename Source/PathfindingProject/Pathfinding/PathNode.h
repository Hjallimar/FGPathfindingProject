#pragma once

#include "Components/SceneComponent.h"
#include "PathNode.generated.h"

USTRUCT()
struct FGridNode
{
	GENERATED_BODY()
public:
	bool bBlocker = false;
	bool bChecked = false;
	int Index = -1;
	float HScore = 1000000.0f;
	float GScore = 1000000.0f;
	TArray<int> NeighbourIndex;
	FGridNode* Parent;
	FORCEINLINE bool operator ==(const FGridNode& Other) const
	{
		return Index == Other.Index;
	}
	FORCEINLINE bool operator >(const FGridNode& Other) const
	{
		return HScore > Other.HScore;
	}
	FORCEINLINE bool operator <(const FGridNode& Other) const
	{
		return HScore < Other.HScore;
	}
	FORCEINLINE bool operator ==(const FGridNode* Other) const
	{
		return Index == Other->Index;
	}
	FORCEINLINE bool operator >(const FGridNode* Other) const
	{
		return HScore > Other->HScore;
	}
	FORCEINLINE bool operator <(const FGridNode* Other) const
	{
		return HScore < Other->HScore;
	}
};


USTRUCT()
struct FNodeScore
{
	GENERATED_BODY()
public:
	float HScore = 0.0f;
	float GScore = 0.0f;
	float FScore = 0.0f;
	FNodeScore()
	{
		HScore = 0.0f;
		GScore = 0.0f;
		FScore = 0.0f;
	}
};
USTRUCT()
struct FNodeInfo
{
	GENERATED_BODY()
public:
	UPathNode* ParentNode = nullptr;
	UPathNode* Node = nullptr;
	FNodeScore ParentScore;
	FNodeScore NodeScore;
	FNodeInfo()
	{
		Node = nullptr;
		ParentScore = FNodeScore();
		NodeScore = FNodeScore();
	}
};

//TESTING---------------
USTRUCT()
struct FNodeNavigationInfo
{
	GENERATED_BODY()
public:
	FNodeNavigationInfo* Previous;
	UPathNode* Node;
	float HScore = 0.0f;
	float GScore = 0.0f;
	explicit FNodeNavigationInfo()
	{
		Previous = nullptr;
		Node = nullptr;
		HScore = 1000000.0f;
		GScore = 1000000.0f;
	}
	FORCEINLINE bool operator==(const FNodeNavigationInfo* NodeInfo) const
	{
		return Node == NodeInfo->Node;
	}
	FORCEINLINE bool operator<(const FNodeNavigationInfo* NodeInfo) const
	{
		return (HScore + GScore) < (NodeInfo->HScore + NodeInfo->GScore);
	}
	FORCEINLINE bool operator==(const FNodeNavigationInfo &NodeInfo) const
	{
		return Node == NodeInfo.Node;
	}
	FORCEINLINE bool operator<(const FNodeNavigationInfo &NodeInfo) const
	{
		return (HScore + GScore) < (NodeInfo.HScore + NodeInfo.GScore);
	}
};

USTRUCT()
struct FPathInfo
{
	GENERATED_BODY()
public:
	TArray<FNodeNavigationInfo> OpenList;
	TArray<FNodeNavigationInfo> ClosedList;
	UPathNode* StartNode;
	UPathNode* GoalNode;
	FPathInfo()
	{
		OpenList = TArray<FNodeNavigationInfo>{};
		ClosedList = TArray<FNodeNavigationInfo>{};
		StartNode = nullptr;
		GoalNode = nullptr;
	}
	FPathInfo(UPathNode* Start, UPathNode* Goal)
	{
		OpenList = TArray<FNodeNavigationInfo>{};
		ClosedList = TArray<FNodeNavigationInfo>{};
		StartNode = Start;
		GoalNode = Goal;
	}
};
//TESTING---------------OVER

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

	UPROPERTY(VisibleAnywhere)
	int NodeIndex = -1;

	FORCEINLINE bool operator ==(const UPathNode &Other) const
	{
		return NodeIndex == Other.NodeIndex;
	}
	FORCEINLINE bool operator ==(const UPathNode* Other) const
	{
		return NodeIndex == Other->NodeIndex;
	}

	void AddNeighbour(UPathNode* Node);
	void AddDiagonalNeighbour(UPathNode* Node);

	void DrawNode(int i);

	FNodeInfo GetParent();
	void SetParent(UPathNode* NewParent, FNodeScore NewParentScore);

	TArray<FVector> CalculatePath(FNodeInfo* Parent, UPathNode* Goal, float GPath, TArray<UPathNode*> OpenList, TArray<UPathNode*> ClosedList);

	TArray<FVector> GeneratePath();

	UPathNode* CalculatePathToEnd(FNodeInfo* Parent, UPathNode* Goal, float GPath, TArray<UPathNode*> OpenList, TArray<UPathNode*> ClosedList);

	//TESTING--------------------------
	FNodeNavigationInfo* RecursivePathCalculation(FPathInfo* PathInfo);

	TArray<FVector> BuildPathFromInfo(FNodeNavigationInfo* Start);
	//TESTING--------------------------OVER


	void CheckNodes(FNodeInfo* BestFit, float GPath, UPathNode* CheckNode, UPathNode* Goal);
	void GatherNeighbours(TArray<UPathNode*> OpenList, TArray<UPathNode*> ClosedList);
	bool ChangeParents(FNodeScore OldParent, FNodeScore NewParent);
	UPROPERTY()
	FNodeInfo Parent;
	UPROPERTY()
	TArray<UPathNode*> Neighbours;
	UPROPERTY()
	TArray<UPathNode*> DiagonalNeighbours;
private:
	const float StepCost = 10.0f;
};