// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Charcacter/BlastCharacter.h"
#include "Animation/AnimationAsset.h"
#include "WeaponType.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8 {
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_MAX UMETA(DisplayName = "DefaultMax")
};
UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScan UMETA(DisplayName = "Hit Scan Weapon"),
	EFT_Projectile UMETA(DisplayName = "Projectile Weapon"),
	EFT_Shotgun UMETA(DisplayName = "Shotgun Weapon"),
	EFT_MAX UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	void AddAmmo(int32 AmmoToAdd);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UTimelineComponent* RecoilTimeline;
	UPROPERTY(EditAnywhere)
	class UCurveFloat* RecoilTimeFloatCurve;

	FVector TraceEndWithScatter(const FVector& HitTarget);
	
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;
	void SetHUDAmmo();

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;

	
	UPROPERTY(EditAnywhere, Category="Weapon Scatter")
	bool bUseScatter = false;
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category = Combat)
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;

	void EnableCustomDepth(bool bEnable);
	bool bDestroyWeapon = false;
	UPROPERTY(EditAnywhere)
	EFireType FireType;
	
	void Recoil();
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	UPROPERTY(EditAnywhere, Category="Weapon Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category="Weapon Scatter")
	float SphereRadius = 75.f;
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	
	UPROPERTY(EditAnywhere)
	float HeadShotDamageCoe = 1.2f;
	UPROPERTY()
	ABlastCharacter* BlasterOwnerCharacter;

	UPROPERTY()
	ABlasterPlayerController* BlasterOwnerController;
	UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind = false;
	
	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);
private:
	UPROPERTY(VisibleAnywhere, Category= "Weapen Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapen Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;
	
	UFUNCTION()
	void OnRep_WeaponState();
	UPROPERTY(VisibleAnywhere, Category = "Weapen Properties")
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapen Properties")
	UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;

	UPROPERTY(EditAnywhere)
	int32 Ammo;

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);
	
	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);
	
	void SpendRound();
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	int32 Sequence = 0;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	FOnTimelineFloat UpdateFunctionFloat;
	UFUNCTION()
	void UpdateRecoilTimeLine(float Output);
public:	
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsEmpty();
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE bool IsFull() const { return Ammo == MagCapacity; }
	FORCEINLINE float GetDamage() const {return Damage;}
	FORCEINLINE float GetHeadShotDamageCoe() const {return HeadShotDamageCoe;}
};
