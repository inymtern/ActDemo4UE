// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SlateWrapperTypes.h"
#include "MyCharacter.generated.h"


class AHookable;
class APostProcessVolume;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UNiagaraComponent;
class UNiagaraSystem;
struct FInputActionValue;

UENUM()
enum class ERightWeaponType : uint8
{
	Hook,
	Throw,
	Shoot
};

UENUM()
enum class ETimeSlowType : uint8
{
	Global,
	Custom
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShowWidget, const int, WidgetIndex, const ESlateVisibility, bWidgetShow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkillBack, const int, SkillIndex);

UCLASS()
class INYMTERNSDEMO_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/**
	 * 手部特效
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Niagara, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* HandTrail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Niagara, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* HookLine;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill_A, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* SkillANiagara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump_2, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* Jump2Niagara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ATK, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* AtkNiagara;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpeedUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Skill_A_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Atk_01_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Atk_02_Action;

	
	

	// move
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Move, meta = (AllowPrivateAccess = "true"))
	bool bCanMove = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Move, meta = (AllowPrivateAccess = "true"))
	float Max_Walk_Speed = 450.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Move, meta = (AllowPrivateAccess = "true"))
	float Min_Walk_Speed = 250.f;
	

	// jump
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = "true"))
	int JpCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = "true"))
	int MaxJpCount = 2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = "true"))
	float NormalJumpForce = 120000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = "true"))
	float JumpForceXYThreshold = .5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Jump, meta = (AllowPrivateAccess = "true"))
	bool bJumpStatus = false;

	// shoot sett
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShootMode, meta = (AllowPrivateAccess = "true"))
	bool bShootMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShootMode, meta = (AllowPrivateAccess = "true"))
	 float ToggleDuration = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShootMode, meta = (AllowPrivateAccess = "true"))
	 float AddTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShootMode, meta = (AllowPrivateAccess = "true"))
	 float Max_Target_Length = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShootMode, meta = (AllowPrivateAccess = "true"))
	 float Min_Target_Length = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShootMode, meta = (AllowPrivateAccess = "true"))
	FVector CameraBoomStartVector = FVector(0,0,0);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ShootMode, meta = (AllowPrivateAccess = "true"))
	FVector CameraBoomEndVector = FVector(0,50,70);

	// hook
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill_Hook, meta = (AllowPrivateAccess = "true"))
	int HookDistance = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill_Hook, meta = (AllowPrivateAccess = "true"))
	float HookForceZ = 100000.f;
	float HookForceXY = 120000.f;

	// 右手 weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	ERightWeaponType RightWeaponType;

	// skill_A
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill_A, meta = (AllowPrivateAccess = "true"))
	bool bSkillA_Active = false;
	// 时停
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill_A, meta = (AllowPrivateAccess = "true"))
	bool bGlobalTimeSlow = false;
	UPROPERTY(VisibleAnywhere)
	APostProcessVolume* PostProcess;

	// ATK_01
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ATK, meta = (AllowPrivateAccess = "true"))
	bool bAtk_01_Active = false;
	// ATK_02
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ATK, meta = (AllowPrivateAccess = "true"))
	bool bAtk_02_Active = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ATK, meta = (AllowPrivateAccess = "true"))
	FVector FinalHitLocation;

	
public:
	AMyCharacter();
	// 通知蓝图显示ui
	UPROPERTY(BlueprintAssignable)
	FShowWidget OnShowWidget;
	UPROPERTY(BlueprintAssignable)
	FSkillBack OnSkillBack;

protected:
	virtual void BeginPlay() override;
	void Move(const FInputActionValue& Value); // 移动
	void Look(const FInputActionValue& Value); // 视角
	
	virtual void SpeedUp(); // 加速
	virtual void SpeedDown(); // 减速
	
	virtual void MyJump(const FInputActionValue& Value); // 按下跳跃
	
	virtual void EnableShootMode(); // 开启瞄准
	virtual void DisableShootMode(); // 关闭瞄准

	virtual void LeftClick(); // 按下左键
	
	virtual void Skill_A();  // 按下Q技能
	
	void ATK_01(); // 按下 F 技能
	void ATK_02(); // 按下 E 技能
	
	void Break_Atk_01(); // 打断 F
	
	void EnterShootModeCallBack(); // 完全进入瞄准模式回调
	void LeaveShootModeCallBack(); // 完全离开瞄准模式回调
	void EnableTimeSlow(ETimeSlowType Type); // 开启时停
	void DisableTimeSlow(ETimeSlowType Type); // 关闭时停



	
	void GetForwardAndRightVectors(FVector& ForwardVector, FVector& RightVector) const;
	FVector GetActorForwardVector(const int Distance) const;
	FVector GetControlForwardVector(const int Distance) const; 
	bool HookLineTrace(FHitResult& HitResult); // 射线检测
public:

	UFUNCTION(BlueprintCallable)
	void MyStopJumping();

	// 落地了
	UFUNCTION(BlueprintCallable)
	void FallingDown();

	// 应用跳跃力
	UFUNCTION(BlueprintCallable)
	void ApplyJumpforce();

	// 保持/清除浮空
	UFUNCTION(BlueprintCallable)
	void KeepAir(bool bKeep);

	UFUNCTION(BlueprintCallable)
	void ApplyATK(); // 触发ATK
	
	


	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
