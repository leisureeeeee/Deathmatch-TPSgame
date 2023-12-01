// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../HUD/BlastHUD.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Weapon/WeaponType.h"
#include "Blaster/BlasterType/CombatState.h"
#include "CombatComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	  
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	friend class ABlastCharacter;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	void EquipWeapon(class AWeapon* WeaponToEquip);
	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);
	bool bLocallyReloading  = false;
protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);
	
	UFUNCTION(server, reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireButtonPressed(bool bPressed);

	void Fire();

	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgun();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	void ShotgunLocalFire(const TArray<FVector_NetQuantize>& TraceHitTargets);
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(Server, Reliable)
	void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);
	
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReoload();
	
	int32 AmountToReload();
private:
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;
	
	UPROPERTY()
	class ABlastCharacter* Character;
	UPROPERTY()
	class ABlasterPlayerController* Controller;
	UPROPERTY()
	class ABlastHUD* HUD;

	UPROPERTY(ReplicatedUsing=OnRep_Aiming)
	bool bAiming = false;

	bool bAimButtonPressed = false;
	UFUNCTION()
	void OnRep_Aiming();
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	FVector HitTarget;

	FHUDPackage HUDPackage;

	float DefaultFOV;

	UPROPERTY(EditAnywhere,Category = Combat)
	float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	FTimerHandle FireTimer;


	bool bCanFire = true;

	void StartFireTimer();
	void FireTimerFinished();

	bool CanFire();

	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;
	
	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;
	
	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 15;

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 15;

	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 15;
	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 15;
	void InitalizeCarriedAmmo();

	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	
	UFUNCTION()
	void OnRep_CombatState();
	
	void UpdateAmmoValues();
	void UpdateCarriedAmmo();
};



