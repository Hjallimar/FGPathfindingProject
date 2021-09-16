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
	int VerticalValue = 0;
	UPROPERTY(VisibleAnywhere)
	int HorizontalValue = 0;
	UPROPERTY(VisibleAnywhere)
	FVector Position;
	UPROPERTY(VisibleAnywhere)
	float NodeSize = 0;
	UPROPERTY(VisibleAnywhere)
	bool blocked = false;
	UPROPERTY(VisibleAnywhere)
	float PathMultiplier = 1.0f;

	void DrawNode();

	UPathNode* NorthWest;
	UPathNode* North;
	UPathNode* NorthEast;
	UPathNode* East;
	UPathNode* SouthEast;
	UPathNode* South;
	UPathNode* SouthWest;
	UPathNode* West;
private:
};