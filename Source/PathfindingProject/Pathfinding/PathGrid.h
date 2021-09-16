#pragma once

#include "GameFramework/Actor.h"
#include "PathGrid.generated.h"

class UPathNode;

UCLASS()
class APathGrid : public AActor
{
	GENERATED_BODY()
public:
	APathGrid();
	UPROPERTY(EditAnywhere, Category = Grid)
	int Rows = 5;
	UPROPERTY(EditAnywhere, Category = Grid)
	int Collums = 5;

	int AreaWidth = 500;
	int AreaLength = 500;
	int AreaHeight = 500;

	UPROPERTY(EditAnywhere, Category = Grid)
	float GridNodeSize = 50.0f;

	UFUNCTION(BlueprintCallable)
	int ConvertPositionToNodeIndex(FVector Position);
	UFUNCTION(BlueprintCallable)
	FVector ConvertNodeIndexToPosition(int index);
	UFUNCTION(BlueprintCallable)
	void DrawGrid();

	UFUNCTION(BlueprintCallable)
	void SwitchBlocked(int i);
	UFUNCTION(BlueprintCallable)
	void UpdateCurrentNode(int i);
	UPathNode* CurrentNode;

private:
	UPROPERTY()
	TArray<UPathNode*> GridBoard;
	void GenerateGrid();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
};