
#include "PathGridBlocker.h"
#include "PathGrid.h"
#include "PathNode.h"
#include "Kismet/GameplayStatics.h"

UPathGridBlocker::UPathGridBlocker()
{
}

void UPathGridBlocker::BeginPlay()
{
	Super::BeginPlay();
	BlockInfo.CenterPosition = GetComponentLocation();
	Cast<APathGrid>(GetOwner())->AddBlocker(this);
}
