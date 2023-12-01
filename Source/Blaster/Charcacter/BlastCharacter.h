// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "../BlasterType/TurnInPlace.h"
#include "Camera/CameraComponent.h"
#include "../Interfaces/IntrractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterType/CombatState.h"
#include "BlastCharacter.generated.h"

UCLASS()
class BLASTER_API ABlastCharacter : public ACharacter, public IIntrractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlastCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();
	void PlayReloadMontage();
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	virtual void OnRep_ReplicatedMovement() override;
	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();
	void SpawnDefaultWeapon();
	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;
protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;


	//Poll for any relevant classes and initialize or HUD
	void PollInit();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtionReleased();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void PlayHitReactMontage();
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamegeType, class AController* InstigatorController, AActor* DamageCauser);
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* head;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* backpack;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* blanket;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;

private:
	UPROPERTY(VisibleAnywhere, CateGory = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;
	
	UPROPERTY(VisibleAnywhere)
	class UBuffComponent* Buff;
	
	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;
	
	UFUNCTION(server, reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ElimMontage;

		UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ReloadMontage;

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	
	bool bRotateRootBone;
	float TurnThershold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere ,Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere ,Category = "Player Stats")
	float Shield  = 0.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);
	
	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	void ElimTimerFinished();

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;
	UFUNCTION()
	void UpdateDissolveMeterial(float DissolveValue);

	void StartDissolve();

	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;
	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	float FallingTime = 0.f;

	UFUNCTION(Server, Reliable)
	void ServerFallingElim(ABlasterPlayerController* PlayerController);
public:	
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera2; }
	FORCEINLINE bool ShouldRotateRootBone() const{ return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	ECombatState GetCombatState() const;
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	FORCEINLINE UCameraComponent* GetCamera() const { return FollowCamera2; }
};
