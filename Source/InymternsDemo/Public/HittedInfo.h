#pragma once
#include "HittedInfo.generated.h"

USTRUCT(BlueprintType)
struct FHittedInfo
{
	GENERATED_BODY();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector HitDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float HitForce;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector FocusPosition; // 强行绑定到
	
};
