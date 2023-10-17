// Fill out your copyright notice in the Description page of Project Settings.


#include "InymternsDemo/Character/Public/MyCharacter.h"

#include "Enemy.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Hookable.h"
#include "InputActionValue.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Engine/PostProcessVolume.h"
#include "InymternsDemo/MyUtils.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 3.f;
	CameraBoom->CameraLagMaxDistance = 120.f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	
	// 发射线条
	HookLine = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HookLine"));
	HookLine->SetupAttachment(RootComponent);

	SkillANiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillA"));
	SkillANiagara->SetupAttachment(GetMesh());
	
	// 手部拖尾
	HandTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HandTrail"));
	HandTrail->SetupAttachment(RootComponent);

	Jump2Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Jump2"));
	Jump2Niagara->SetupAttachment(GetMesh());

	AtkNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ATK"));
	AtkNiagara->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	HookLine->SetAutoActivate(false);
	HandTrail->SetAutoActivate(false);
	SkillANiagara->SetAutoActivate(false);
	Jump2Niagara->SetAutoActivate(false);
	AtkNiagara->Deactivate();
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

    // 附加拖尾到手上
	HandTrail->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("HandTrailSocket"));
	HookLine->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("HandTrailSocket"));
	
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	if(bCanMove)
	{
		FVector2D MovementVector = Value.Get<FVector2D>();
		if (Controller != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
	
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMyCharacter::SpeedUp()
{
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->MaxWalkSpeed = Max_Walk_Speed;
}

void AMyCharacter::SpeedDown()
{
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->MaxWalkSpeed = Min_Walk_Speed;
}

void AMyCharacter::PressJump()
{
	if(!bJumpStatus && ( JpCount == 0 || JpCount == 1 ) )
	{
		bJumpStatus = true;
		JpCount++;
	}
}


void AMyCharacter::EnableShootMode()
{
	bShootMode = true;
	CameraSmoothToggle(true);
}

void AMyCharacter::DisableShootMode()
{
	bShootMode = false;
	DisableTimeSlow(ETimeSlowType::Custom);
	CameraSmoothToggle(false);
}



void AMyCharacter::PressLeftClick()
{
	if(bShootMode && RightWeaponType == ERightWeaponType::Hook)
	{
		DisableShootMode();
		FHitResult HitResult;
		if(bool bHit = HookLineTrace(HitResult))
		{
			AActor* Actor = HitResult.GetActor();
			if(Cast<AHookable>(Actor))
			{
				UMyUtils::Debug(Actor->GetName());
				HookLine->SetActive(true,true);
				HookLine->SetVariableVec3(FName("EndPosition"), HitResult.Location);
				const FRotator Rotation = ( HitResult.Location - GetActorLocation() ).Rotation();
				FVector ForwardVector = UMyUtils::GetForwardVector(Rotation);
				SetActorRotation(Rotation);
				ForwardVector *= HookForceXY;
				if(JpCount == 0)
				{
					JpCount = 1;
				}
				GetCharacterMovement()->Velocity = FVector(0,0,0);
				GetCharacterMovement()->AddImpulse(FVector(ForwardVector.X, ForwardVector.Y, HookForceZ), false);
			}
		}
	}else if(bShootMode && RightWeaponType == ERightWeaponType::Hook)
	{
		
	}
}



void AMyCharacter::PressSkillA()
{
	if(GetCharacterMovement()->IsFalling() && !bSkillA_Active )
	{
		bSkillA_Active = true;
		bJumpStatus = false;
		CameraBoom->TargetArmLength = 500.f;
		JpCount = JpCount == 0 ? 1 : JpCount;
		KeepAir(true);
		TArray<AActor*> Array;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Array);
		const FVector CurrentLocation = GetActorLocation();
		for (const AActor* Actor : Array)
		{
			const FVector ActorLocation = Actor->GetActorLocation();
			UNiagaraComponent* SpawnSystemAtLocation = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SkillANiagara->GetAsset(), CurrentLocation);
			SpawnSystemAtLocation->SetVariableVec3(FName("TargetPosition"), ActorLocation);
		}
	}
}

void AMyCharacter::PressAtk1()
{
	if(bShootMode && !bAtk_01_Active && !bSkillA_Active) // 代表使用远程
	{
		
		FHitResult HitResult;
		bool bHit = HookLineTrace(HitResult);
		if(!bHit) return;
		FinalHitLocation = HitResult.Location;
		switch (RightWeaponType)
		{
		case ERightWeaponType::Hook: // 钩子
			{
				DisableShootMode();
				bAtk_01_Active = true;
				KeepAir(true);
				break;
			}
		case ERightWeaponType::Throw: // 投掷物
			{
				break;
			}
		case ERightWeaponType::Shoot: // 射击
			{
				break;
			}	
		default: return;
		}
	}
}

void AMyCharacter::PressAtk2()
{
}


void AMyCharacter::EnterShootModeCallBack()
{
	bUseControllerRotationYaw = true;
	EnableTimeSlow(ETimeSlowType::Custom);
	if(AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass()))
	{
		PostProcess = Cast<APostProcessVolume>(Actor);
		if(PostProcess)
		{
			PostProcess->bEnabled = true;
		}
	}
	OnShowWidget.Broadcast(0, ESlateVisibility::Visible);
}

void AMyCharacter::LeaveShootModeCallBack()
{
	bUseControllerRotationYaw = false;
	// DisableTimeSlow(ETimeSlowType::Custom);
	if(PostProcess)
	{
		PostProcess->bEnabled = false;
	}
	OnShowWidget.Broadcast(0, ESlateVisibility::Hidden);
}

void AMyCharacter::EnableTimeSlow(const ETimeSlowType Type)
{
	if(GetCharacterMovement()->IsFalling())
	{
		bGlobalTimeSlow = true;
		switch (Type)
		{
		case ETimeSlowType::Global:
			{
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1);
				break;
			}
		case ETimeSlowType::Custom:
			{
				// TODO
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1);
			}
		default: return;
		}
	}
}

void AMyCharacter::DisableTimeSlow(const ETimeSlowType Type)
{
	bGlobalTimeSlow = false;
	switch (Type)
	{
	case ETimeSlowType::Global:
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
			break;
		}
	case ETimeSlowType::Custom:
		{
			// TODO
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
			break;
		}
	default: return;
	}
}

void AMyCharacter::CameraSmoothToggle(bool bZoomUp)
{
	if (!GetWorldTimerManager().IsTimerActive(ShootTimerHandle))
	{
		ShootTimerDelegate.BindLambda([this, bZoomUp]()
		{
			SetCameraTransition(bZoomUp);
		});
		GetWorldTimerManager().SetTimer(ShootTimerHandle, ShootTimerDelegate, DT, true);
	}else
	{
		GetWorldTimerManager().ClearTimer(ShootTimerHandle);
		AddTime = ToggleDuration - AddTime;
		ShootTimerDelegate.BindLambda([this, bZoomUp]()
		{
			SetCameraTransition(bZoomUp);
		});
		GetWorldTimerManager().SetTimer(ShootTimerHandle, ShootTimerDelegate, DT, true);
	}
	
}

void AMyCharacter::SetCameraTransition(const bool bZoomUp)
{
	const float DeltaTargetLength = FMath::Lerp(bZoomUp ? Max_Target_Length : Min_Target_Length, bZoomUp ? Min_Target_Length : Max_Target_Length, AddTime / ToggleDuration);
	const FVector DeltaCameraVector = FMath::Lerp(bZoomUp ? CameraBoomStartVector : CameraBoomEndVector, bZoomUp ? CameraBoomEndVector : CameraBoomStartVector, AddTime / ToggleDuration);
	CameraBoom->TargetArmLength = DeltaTargetLength;
	CameraBoom->SetRelativeLocation(DeltaCameraVector);
	AddTime += DT;
	if(AddTime >= ToggleDuration)
	{
		AddTime = 0.f;
		GetWorldTimerManager().ClearTimer(ShootTimerHandle);
		if(bZoomUp)
		{
			EnterShootModeCallBack();
		}else
		{
			LeaveShootModeCallBack();
		}
	}
}


void AMyCharacter::KeepAir(const bool bKeep) 
{
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	if(bKeep)
	{
		bCanMove = false;
		CharacterMovementComponent->GravityScale = 0.f;
		CharacterMovementComponent->Velocity = FVector(0.f, 0.f, 0.f);
	}else
	{
		bCanMove = true;
		CharacterMovementComponent->GravityScale = Default_Gravity;
	}
}



FVector AMyCharacter::GetControlForwardVector(const int Distance) const
{
	const FRotator ControlRotation = GetControlRotation();
	const FVector ActorLocation = GetActorLocation();
	const FRotationMatrix RotationMatrix(ControlRotation);
	const FVector ForwardVector = RotationMatrix.GetScaledAxis(EAxis::X);
	return ActorLocation + ForwardVector * Distance;
}



bool AMyCharacter::HookLineTrace(FHitResult& HitResult) const
{
	const UWorld* World = GetWorld();
	const APlayerController* FirstPlayerController = World->GetFirstPlayerController();
	FVector Pos, Dir;
	const FVector2D ScreenSize = UMyUtils::GetScreenSize();
	FirstPlayerController->DeprojectScreenPositionToWorld(ScreenSize.X / 2, ScreenSize.Y / 2, Pos, Dir);
	const FVector End = Pos + Dir * TraceDistance;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	const bool bHit = World->LineTraceSingleByChannel(HitResult, Pos, End, ECC_Pawn, CollisionQueryParams);
	return bHit;
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::PressJump);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		EnhancedInputComponent->BindAction(SpeedUpAction, ETriggerEvent::Started, this, &AMyCharacter::SpeedUp);
		EnhancedInputComponent->BindAction(SpeedUpAction, ETriggerEvent::Completed, this, &AMyCharacter::SpeedDown);
		EnhancedInputComponent->BindAction(ShootModeAction, ETriggerEvent::Started, this, &AMyCharacter::EnableShootMode);
		EnhancedInputComponent->BindAction(ShootModeAction, ETriggerEvent::Completed, this, &AMyCharacter::DisableShootMode);
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &AMyCharacter::PressLeftClick);
		EnhancedInputComponent->BindAction(Skill_A_Action, ETriggerEvent::Started, this, &AMyCharacter::PressSkillA);
		EnhancedInputComponent->BindAction(Atk_01_Action, ETriggerEvent::Started, this, &AMyCharacter::PressAtk1);
		EnhancedInputComponent->BindAction(Atk_02_Action, ETriggerEvent::Started, this, &AMyCharacter::PressAtk2);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, TEXT("BindAction failed"));
	}
}

void AMyCharacter::GetForwardAndRightVectors(FVector& ForwardVector, FVector& RightVector) const
{
	const FRotator ControlRotation = GetControlRotation();
	const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);
	ForwardVector = ControlRotationYaw.Vector();
	RightVector = ForwardVector.RotateAngleAxis(90.f, FVector(0.f, 0.f, 1.f));
}

FVector AMyCharacter::GetActorForwardVector(const int Distance) const
{
	const FRotator ControlRotation = GetActorRotation();
	const FVector ActorLocation = GetActorLocation();
	const FRotationMatrix RotationMatrix(ControlRotation);
	const FVector ForwardVector = RotationMatrix.GetScaledAxis(EAxis::X);
	return ActorLocation + ForwardVector * Distance;
}

