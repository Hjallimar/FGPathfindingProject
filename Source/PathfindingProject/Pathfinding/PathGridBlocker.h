#pragma once

#include "Components/SceneComponent.h"
#include "PathGridBlocker.generated.h"


USTRUCT(BlueprintType)
struct FBlockerInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool Block = true;
	UPROPERTY(EditAnywhere)
	float Multiplier = 1.0f;
	UPROPERTY(EditAnywhere)
	float Width = 100.f;
	UPROPERTY(EditAnywhere)
	float Height = 100.f;
	UPROPERTY(EditAnywhere)
	float Length = 100.f;
	UPROPERTY()
	FVector CenterPosition = FVector::ZeroVector;
};

UCLASS(meta=(BlueprintSpawnableComponent))
class UPathGridBlocker : public USceneComponent
{
	GENERATED_BODY()
public:
	UPathGridBlocker();

	UPROPERTY(EditAnywhere)
	FBlockerInfo BlockInfo;

	virtual void BeginPlay() override;
};