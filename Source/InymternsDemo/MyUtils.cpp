// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUtils.h"
#include "DrawDebugHelpers.h"

FVector2D UMyUtils::GetScreenSize()
{
	UGameViewportClient* ViewportClient = GEngine->GameViewport;
	if (ViewportClient != nullptr)
	{
		FViewport* Viewport = ViewportClient->Viewport;
		if (Viewport != nullptr)
		{
			return FVector2D(Viewport->GetSizeXY());
		}
	}
	// 如果无法获取屏幕大小，则返回默认值
	return FVector2D::ZeroVector;
}

void UMyUtils::Debug(const FString& Str)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, Str);
}

void UMyUtils::DrawLine(UWorld* World, FVector Start, FVector End)
{
	if (World != nullptr)
	{
		DrawDebugLine(
			World,
			Start,
			End,
			FColor::Blue,
			false,
			1.f,
			0,
			1.f
		);
	}
}

FVector UMyUtils::GetForwardVector(FRotator Rotator)
{
	FRotationMatrix RotationMatrix(Rotator);
	return RotationMatrix.GetScaledAxis(EAxis::X);
}
