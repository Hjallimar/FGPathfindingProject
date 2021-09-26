#pragma once

#include "Components/SceneComponent.h"
#include "PathNode.generated.h"

USTRUCT()
struct FNodeNavigationInfo
{
	GENERATED_BODY()
public:
	FNodeNavigationInfo* Previous = nullptr;
	UPathNode* Node = nullptr;
	float HScore = 0.0f;
	float GScore = 0.0f;
	explicit FNodeNavigationInfo()
	{
		Previous = nullptr;
		Node = nullptr;
		HScore = 0.0f;
		GScore = 0.0f;
	}
	explicit FNodeNavigationInfo(UPathNode* NewNode)
	{
		Previous = nullptr;
		Node = NewNode;
		HScore = 0.0f;
		GScore = 0.0f;
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

UCLASS()
class UPathNode : public USceneComponent
{
	GENERATED_BODY()
public:
	UPathNode();
	//Variables
	UPROPERTY(VisibleAnywhere)
	FVector Position;
	UPROPERTY(VisibleAnywhere)
	int NodeIndex = -1;
	UPROPERTY(VisibleAnywhere)
	bool blocked = false;
	UPROPERTY(VisibleAnywhere)
	float PathMultiplier = 1.0f;
	UPROPERTY()
	TArray<UPathNode*> Neighbours;
	UPROPERTY()
	TArray<UPathNode*> DiagonalNeighbours;

	//Operators
	FORCEINLINE bool operator ==(const UPathNode &Other) const
	{
		return NodeIndex == Other.NodeIndex;
	}
	FORCEINLINE bool operator ==(const UPathNode* Other) const
	{
		return NodeIndex == Other->NodeIndex;
	}
};