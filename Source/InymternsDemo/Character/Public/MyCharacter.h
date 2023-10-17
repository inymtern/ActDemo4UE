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
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpeedUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Skill_A_Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Atk_01_Action;
	

	// move
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	bool bCanMove = true;
	

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ATK, meta = (AllowPrivateAccess = "true"))
	FVector FinalHitLocation;

	
	
public:
	// Sets default values for this character's properties
	AMyCharacter();

	const float Max_Walk_Speed = 450.f;
	const float Min_Walk_Speed = 250.f;

	// 通知蓝图显示ui
	UPROPERTY(BlueprintAssignable)
	FShowWidget OnShowWidget;
	UPROPERTY(BlueprintAssignable)
	FSkillBack OnSkillBack;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	virtual void SpeedUp();
	virtual void SpeedDown();
	/** 跳跃 **/
	virtual void MyJump(const FInputActionValue& Value) ;
	// 瞄准模式
	virtual void EnableShootMode();
	virtual void DisableShootMode();
	// 钩子
	virtual void Hook();
	// 一技能
	virtual void Skill_A();
	// 普通远程攻击
	void ATK_01();
	// 打断
	void Break_Atk_01();
	

	void EnterShootModeCallBack();
	
	void LeaveShootModeCallBack();

	void EnableTimeSlow(ETimeSlowType Type);
	void DisableTimeSlow(ETimeSlowType Type);
	
	
	

public:


	// 可进行下次跳跃
	UFUNCTION(BlueprintCallable)
	void MyStopJumping() ;

	// 落地了
	UFUNCTION(BlueprintCallable)
	void FallingDown();
	

	// 应用跳跃力
	UFUNCTION(BlueprintCallable)
	void ApplyJumpforce();

	// 保持清除浮空
	UFUNCTION(BlueprintCallable)
	void KeepAir(bool bKeep) ;

	UFUNCTION(BlueprintCallable)
	void ApplyATK();
	
	void GetForwardAndRightVectors(FVector& ForwardVector, FVector& RightVector) const;

	FVector GetActorForwardVector(const int Distance) const;

	FVector GetControlForwardVector(const int Distance) const;

	bool HookLineTrace(FHitResult& HitResult);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }



};
