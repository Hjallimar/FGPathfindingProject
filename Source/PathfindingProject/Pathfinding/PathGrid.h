#pragma once

#include "GameFramework/Actor.h"
#include "PathGridBlocker.h"

#include "PathGrid.generated.h"

class UPathNode;
class ULineBatchComponent;

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
	float HalfNodeSize = 0.0f;

	UFUNCTION(BlueprintCallable)
	int ConvertPositionToNodeIndex(FVector Position);
	UFUNCTION(BlueprintCallable)
	FVector ConvertNodeIndexToPosition(int index);
	UFUNCTION(BlueprintCallable)
	void DrawGrid();

	UFUNCTION(BlueprintCallable)
	TArray<FVector> CalculatePath(FVector StartPos, FVector EndPos);


	UFUNCTION(BlueprintCallable)
	void SwitchBlocked(int i);
	UFUNCTION(BlueprintCallable)
	void UpdateCurrentNode(int i);
	UFUNCTION(BlueprintCallable)
	void AddBlocker(UPathGridBlocker* Blocker);
	UPathNode* CurrentNode;

	//DebugDrawing
	UPROPERTY()
	ULineBatchComponent* LineBatch = nullptr;
	bool CheckWorldlocationInGrid(const FVector& WorldLocation) const;

private:
	UPROPERTY(Transient)
	TArray<UPathNode*> GridBoard;
	TArray<UPathGridBlocker*> Blockers;
	int PrevRow = 0;
	int PrevCol = 0;
	float TimeSinceLastRender = 0.0f;

	TArray<FVector> CalculatePath(UPathNode* StartNode, UPathNode* EndNode);
	TArray<FVector> CalculatePath(int StartIndex, int EndIndex);

	void UpdatePathBlocks();
	void GenerateGrid();
	bool IsInsideBounderies(FVector position, FVector Center, FVector Bounderies);
	
	TArray<int> JustGiveMePath(UPathNode* StartNode, UPathNode* EndNode);
	float ICry(FVector a, FVector b);


	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual bool ShouldTickIfViewportsOnly() const override;

};