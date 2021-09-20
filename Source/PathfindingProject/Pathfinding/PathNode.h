#pragma once

#include "Components/SceneComponent.h"
#include "PathNode.generated.h"


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
	void CalculatePath(UPathNode* Parent, UPathNode* Goal, TArray<UPathNode*> OpenList, TArray<UPathNode*> ClosedList);

	UPROPERTY()
	UPathNode* Up; 
	UPROPERTY()
	UPathNode* Left; 
	UPROPERTY()
	UPathNode* Down;
	UPROPERTY()
	UPathNode* Right; 
	// X
	UPROPERTY()
	UPathNode* UpLeft; 
	UPROPERTY()
	UPathNode* UpRight; 
	UPROPERTY()
	UPathNode* DownRight; 
	UPROPERTY()
	UPathNode* DownLeft;
private:
};