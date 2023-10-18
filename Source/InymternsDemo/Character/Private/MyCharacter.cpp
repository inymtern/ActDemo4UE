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

	SkillA_NG = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillANG"));
	SkillA_NG->SetupAttachment(RootComponent);

	AtkHook1_NG = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AtkNG"));
	AtkHook1_NG->SetupAttachment(RootComponent);
	
	// 发射线条
	HookLine = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HookLine"));
	HookLine->SetupAttachment(RootComponent);

	
	// 手部拖尾
	HandTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HandTrail"));
	HandTrail->SetupAttachment(RootComponent);

	Jump2Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Jump2"));
	Jump2Niagara->SetupAttachment(GetMesh());




}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 附加拖尾到手上
	const FName SocketName = FName("HandTrailSocket");
	if(HookLine)
	{
		HookLine->bAutoActivate = false;
		HookLine->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	}
	if(HandTrail)
	{
		HandTrail->bAutoActivate = false;
		HandTrail->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	}
	if(Jump2Niagara)
	{
		Jump2Niagara->bAutoActivate = false;
	}
	// if(SkillANG)
	// {
	// 	SkillANG->bAutoActivate = false;
	// 	SkillANG->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	// }
	if(AtkHook1_NG)
	{
		AtkHook1_NG->bAutoActivate = false;
		AtkHook1_NG->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	}

	
	
	
	
	
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

    
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	if(!bCanMove) return;
	if(bAtk_01_Active && !bCanBreakAtk01) return;
	if(bAtk_02_Active && !bCanBreakAtk02) return;
	Break_Atk1();
	Break_Atk2();
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

	// Logic
	if(bAtk_01_Active && !bCanBreakAtk01)
	{
		return;
	};
	if(bAtk_02_Active && !bCanBreakAtk02)
	{
		return;
	};
	if(bSkillA_Active && !bCanBreakSkillA)
	{
		return;
	};
	
	
	if(JpCount == 0 && !bJumpStatus) // 首次跳跃
	{
		bJumpStatus = true;
		JpCount++;
		return;
	}
	if(JpCount == 1 && bJumpStatus && bCanJumpBreak)
	{
		Jump2Niagara->SetActive(true, true);
		Break_JumpStart();
		bJumpStatus = true;
		JpCount++;
		return;
	}
	if(JpCount == 1 && !bJumpStatus)
	{
		if(!GetCharacterMovement()->IsFalling()) // 落地时可打断时才能跳跃
		{
			Break_Land();
			if(bCanLandBreak)
			{
				bJumpStatus = true;
				JpCount++;
			}
		}else
		{
			bJumpStatus = true;
			JpCount++;
			Jump2Niagara->SetActive(true, true);
		}	
	}
}

void AMyCharacter::CanBreak_JumpStart()
{
	bCanJumpBreak = true;
	ApplyJumpForce();
}

void AMyCharacter::End_JumpStart()
{
	bJumpStatus = false;
	bCanJumpBreak = false;
}

void AMyCharacter::Break_JumpStart()
{
	if(bCanJumpBreak)
	{
		bJumpStatus = false;
		bCanJumpBreak = false;
	}
}

void AMyCharacter::ApplyJumpForce()
{
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->Velocity = FVector(0,0,0);
	const APlayerController* FirstPlayerController = GetWorld()->GetFirstPlayerController();
	const FVector ForwardVector = UMyUtils::GetForwardVector(FirstPlayerController->GetControlRotation());
	const FVector RightVector = ForwardVector.RotateAngleAxis(90.f, FVector(0.f, 0.f, 1.f));
	FVector Force;
	if (FirstPlayerController->IsInputKeyDown(EKeys::W))
	{
		Force += ForwardVector;
	}
	if(FirstPlayerController->IsInputKeyDown(EKeys::S))
	{
		Force -= ForwardVector;
	}
	if(FirstPlayerController->IsInputKeyDown(EKeys::D))
	{
		Force += RightVector;
	}
	if(FirstPlayerController->IsInputKeyDown(EKeys::A))
	{
		Force -= RightVector;
	}
	UMyUtils::Debug(Force.ToString());
	Force = FVector(Force.X * NormalJumpForce * JumpForceXYThreshold, Force.Y * NormalJumpForce * JumpForceXYThreshold, NormalJumpForce );
	CharacterMovementComponent->AddImpulse(Force, false);
}

void AMyCharacter::CanBreak_Land()
{
	if(!GetCharacterMovement()->IsFalling())
	{
		bCanLandBreak = true;
	}
}

void AMyCharacter::Break_Land()
{
	JpCount = 0;
	bCanLandBreak = false;
}

void AMyCharacter::End_Land()
{
	bCanLandBreak = false;
	JpCount = 0;
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
		if(bool bHit = HookLineTrace(HitResult, HookDistance)) 
		{
			AActor* Actor = HitResult.GetActor();
			if(AEnemy* Enemy = Cast<AEnemy>(Actor)) // 
			{
				if(const int Mass = Enemy->Mass; Mass > MaxHookMass)
				{
					ApplyHook(HitResult.Location, HookForceZ, true, false);
				}else
				{
					ApplyHook(HitResult.Location, HookForceZ, false, false);
				}
			}else if(Cast<AHookable>(Actor)) // 可勾的物体
			{
				ApplyHook(HitResult.Location, HookForceZ, true, true);
			}
		}else
		{
			
		}
	}else if(bShootMode && RightWeaponType == ERightWeaponType::Throw)
	{
		UMyUtils::Debug("Throw ...", FColor::Green);
	}
}



void AMyCharacter::PressSkillA()
{
	if(bAtk_01_Active && !bCanBreakAtk01) return;
	if(bAtk_02_Active && !bCanBreakAtk02) return;
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
			UNiagaraComponent* SpawnSystemAtLocation = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SkillA_NG->GetAsset(), CurrentLocation);
			SpawnSystemAtLocation->SetVariableVec3(FName("TargetPosition"), ActorLocation);
		}
		SkillATimerDelegate.BindLambda([this]()
		{
			this->End_SkillA();	
		});
		GetWorldTimerManager().SetTimer(SkillATimerHandle, SkillATimerDelegate, SkillA_Duration, false);
	}
}

void AMyCharacter::CanBreak_SkillA()
{
	bCanBreakSkillA = true;
}

void AMyCharacter::Break_SkillA()
{
	if(bCanBreakSkillA)
	{
		bCanBreakSkillA = false;
		bSkillA_Active = false;
		GetWorldTimerManager().ClearTimer(SkillATimerHandle);
		if(!bAtk_01_Active && !bAtk_02_Active)
		{
			KeepAir(false);
		}
	}
}

void AMyCharacter::End_SkillA()
{
	if(!bAtk_01_Active && !bAtk_02_Active)
	{
		KeepAir(false);
	}
	bCanBreakSkillA = false;
	bSkillA_Active = false;
	GetWorldTimerManager().ClearTimer(SkillATimerHandle);
	
}

void AMyCharacter::PressAtk1()
{
	if(bAtk_02_Active && !bCanBreakAtk02) return;
	if(bSkillA_Active && !bCanBreakSkillA) return;
	
	if(bShootMode && !bAtk_01_Active ) // 代表使用远程
	{
		if(GetCharacterMovement()->IsFalling())
		{
			KeepAir(true);
		}
		switch (RightWeaponType)
		{
		case ERightWeaponType::Hook: // 钩子
			{
				bAtk_01_Active = true;
				FHitResult HitResult;
				HookLineTrace(HitResult, Atk_01_Distance);
				TempHitLocation = HitResult.Location;
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

void AMyCharacter::CanBreak_Atk1()
{
	bCanBreakAtk01 = true;
}

void AMyCharacter::Break_Atk1()
{
	if(bAtk_01_Active)
	{
		if(!bSkillA_Active && !bAtk_02_Active)
		{
			KeepAir(false);
		}
		GetWorldTimerManager().ClearTimer(AtkTimerHandle);
		bCanBreakAtk01 = false;
		bAtk_01_Active = false;
		if(AtkHook1_NG && AtkHook1_NG->IsActive())
		{
			AtkHook1_NG->Deactivate();
		}
	}
	
}

void AMyCharacter::End_Atk1()
{
	bCanBreakAtk01 = false;
	bAtk_01_Active = false;
	if(!bSkillA_Active && !bAtk_02_Active)
	{
		KeepAir(false);
	}
	GetWorldTimerManager().ClearTimer(AtkTimerHandle);
	if(AtkHook1_NG && AtkHook1_NG->IsActive())
	{
		AtkHook1_NG->Deactivate();
	}
}

void AMyCharacter::Apply_Atk1()
{
	// 出特效
	// FHitResult HitResult;
	// HookLineTrace(HitResult, Atk_01_Distance);
	// const FVector AtkEnd = HitResult.Location;
	if(AtkHook1_NG)
	{
		AtkHook1_NG->Activate();
		if(TempHitLocation.IsNearlyZero())
		{
			TempHitLocation = GetActorForwardVector(Atk_01_Distance);
		}
		AtkHook1_NG->SetVariableVec3(FName("Location"), TempHitLocation);
	}
	// AtkNiagara->Activate();
	AtkTimerDelegate.BindLambda([this]()
	{
		this->End_Atk1();
	});
	GetWorldTimerManager().SetTimer(AtkTimerHandle,AtkTimerDelegate, Atk_01_Duration, false);
}

void AMyCharacter::PressAtk2()
{
	if(bAtk_01_Active && !bCanBreakAtk01) return;
	if(bSkillA_Active && !bCanBreakSkillA) return;
	if(bShootMode && !bAtk_02_Active ) // 代表使用远程
	{
		if(GetCharacterMovement()->IsFalling())
		{
			KeepAir(true);
		}
		switch (RightWeaponType)
		{
		case ERightWeaponType::Hook: // 钩子
			{
				bAtk_02_Active = true;
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

void AMyCharacter::CanBreak_Atk2()
{
	bCanBreakAtk02 = true;
}

void AMyCharacter::Break_Atk2()
{
	if(bAtk_02_Active)
	{
		if(!bAtk_01_Active && !bSkillA_Active)
		{
			KeepAir(false);
		}
		bCanBreakAtk02 = false;
		bAtk_02_Active = false;
	}
}

void AMyCharacter::End_Atk2()
{
	if(!bAtk_01_Active && !bSkillA_Active)
	{
		KeepAir(false);
	}
	bCanBreakAtk02 = false;
	bAtk_02_Active = false;
}

void AMyCharacter::Apply_Atk2()
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
		GetWorldTimerManager().SetTimer(ShootTimerHandle, ShootTimerDelegate, DT, true, 0);
	}else
	{
		GetWorldTimerManager().ClearTimer(ShootTimerHandle);
		AddTime = ToggleDuration - AddTime;
		ShootTimerDelegate.BindLambda([this, bZoomUp]()
		{
			SetCameraTransition(bZoomUp);
		});
		GetWorldTimerManager().SetTimer(ShootTimerHandle, ShootTimerDelegate, DT, true, 0);
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
		CameraBoom->SetRelativeLocation(bZoomUp ? CameraBoomEndVector : CameraBoomStartVector);
		CameraBoom->TargetArmLength = bZoomUp ? Min_Target_Length : Max_Target_Length;
		if(bZoomUp)
		{
			EnterShootModeCallBack();
		}else
		{
			LeaveShootModeCallBack();
		}
	}
}

void AMyCharacter::ApplyHook(FVector Location, float Force, bool bPullSelf, bool bStatic)
{
	HookLine->SetActive(true,true);
	HookLine->SetVariableVec3(FName("EndPosition"), Location);
	const FRotator Rotation = ( Location - GetActorLocation() ).Rotation();
	const FVector ForwardVector = UMyUtils::GetForwardVector(Rotation);
	SetActorRotation(Rotation);
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->Velocity = FVector(0.f, 0.f, 0.f);
	if(JpCount == 0)
	{
		JpCount = 1;
	}
	if(bStatic)
	{
		if(CharacterMovementComponent->IsFalling())
		{
			CharacterMovementComponent->AddImpulse(FVector(ForwardVector.X * Force, ForwardVector.Y * Force, HookSelfToStaticZThreshold * 0.8f * Force), false);
		}else
		{
			CharacterMovementComponent->AddImpulse(FVector(ForwardVector.X * Force, ForwardVector.Y * Force, HookSelfToStaticZThreshold * Force), false);
		}
	}else
	{
		if(bPullSelf) // 拉自己过去
		{
			CharacterMovementComponent->AddImpulse(FVector(ForwardVector.X * Force, ForwardVector.Y * Force, HookSelfToEnemyZThreshold * Force), false);
		}else // 将敌人拉过来
		{
			UMyUtils::Debug("pull enemy");
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



bool AMyCharacter::HookLineTrace(FHitResult& HitResult, float Distance) const
{
	const UWorld* World = GetWorld();
	const APlayerController* FirstPlayerController = World->GetFirstPlayerController();
	FVector Pos, Dir;
	const FVector2D ScreenSize = UMyUtils::GetScreenSize();
	FirstPlayerController->DeprojectScreenPositionToWorld(ScreenSize.X / 2, ScreenSize.Y / 2, Pos, Dir);
	const FVector End = Pos + Dir * Distance;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	const bool bHit = World->LineTraceSingleByChannel(HitResult, Pos, End, ECC_Pawn, CollisionQueryParams);
	return bHit;
}

FVector AMyCharacter::CalcAtkEnd(const float Distance) const
{
	const UWorld* World = GetWorld();
	const APlayerController* FirstPlayerController = World->GetFirstPlayerController();
	FVector Pos, Dir;
	const FVector2D ScreenSize = UMyUtils::GetScreenSize();
	FirstPlayerController->DeprojectScreenPositionToWorld(ScreenSize.X / 2, ScreenSize.Y / 2, Pos, Dir);
	return Pos + Dir * Distance;
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

