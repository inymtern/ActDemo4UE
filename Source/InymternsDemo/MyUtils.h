// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUtils.generated.h"

/**
 * 
 */
UCLASS()
class INYMTERNSDEMO_API UMyUtils : public UObject
{
	GENERATED_BODY()

public:
	static FVector2D GetScreenSize();
	static void Debug(const FString& Str);
	static void DrawLine(UWorld* World, FVector Start, FVector End);
	static FVector GetForwardVector(FRotator Rotator);
};
