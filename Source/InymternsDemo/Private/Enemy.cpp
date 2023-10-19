// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "HittedInfo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InymternsDemo/MyUtils.h"


// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::Hit(FHittedInfo& HitIf)
{
	bHitted = true;
	this->HitInfo = HitIf;
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	FVector ActorLocation = GetActorLocation();
	FRotator Rotation = (ActorLocation - HitIf.HitDirection).Rotation();
	FVector ForwardVector = UMyUtils::GetForwardVector(Rotation);
	if(HitIf.HitForce > 2)
	{
		// bHitted = false;
		CharacterMovementComponent->AddImpulse(FVector(ForwardVector.X * HitIf.HitForce * HitForceRate, ForwardVector.Y * HitIf.HitForce * HitForceRate, 0.7f * HitIf.HitForce * HitForceRate));
	}else
	{
		if(CharacterMovementComponent->IsFalling())
		{
			if(!HitIf.FocusPosition.IsNearlyZero())
			{
				
				SetActorLocation(HitIf.FocusPosition);
			}
			CharacterMovementComponent->Velocity = FVector(0,0,0);
		}
		CharacterMovementComponent->AddImpulse(FVector(ForwardVector.X  , ForwardVector.Y  , 0.2f  * HitForceRate));
	}
}

void AEnemy::End_Hit()
{
	bHitted = false;
	this->HitInfo.HitForce = 0.f;
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
}

