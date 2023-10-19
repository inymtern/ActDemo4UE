// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HittedInfo.h"
#include "Hookable.h"
#include "Enemy.generated.h"

UCLASS()
class INYMTERNSDEMO_API AEnemy : public AHookable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FHittedInfo HitInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHitted = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HitForceRate = 45000.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Hit(FHittedInfo& HitInfo);
	UFUNCTION(BlueprintCallable)
	virtual void End_Hit();
};
