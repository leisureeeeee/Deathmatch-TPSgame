// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/Charcacter/BlastCharacter.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"


USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector Location;
	
	UPROPERTY()
	FRotator Rotation;
	
	UPROPERTY()
	FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()
	UPROPERTY()
	float Time;
	
	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;
	
	UPROPERTY()
	ABlastCharacter* Character;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitconfirmed;

	UPROPERTY()
	bool bHeadShot;

};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<ABlastCharacter*, uint32> HeadShots;
	UPROPERTY()
	TMap<ABlastCharacter*, uint32> BodyShots;

};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULagCompensationComponent();
	friend class ABlastCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);

	/*
	 * HitScan
	 */
	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		ABlastCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime,
		class AWeapon* DamageCauser
	);
	

	/*
	 * Projectile
	 */
	UFUNCTION(Server, Reliable)
	void ProjectileServerScoreRequst(
		ABlastCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime,
		AWeapon* DamageCauser
		);
	/*
	 * Shotgun
	 */
	
	UFUNCTION(Server, Reliable)
	void ServerShotgunServerScoreRequest(
	const TArray<ABlastCharacter*>& HitCharacters,
	const FVector_NetQuantize& TraceStart,
	const TArray<FVector_NetQuantize>& HitLocations,
	float HitTime,
	AWeapon* DamageCauser
	);

protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);

	void CacheBoxPositions(ABlastCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(ABlastCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(ABlastCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(ABlastCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(ABlastCharacter* HitCharacter, float HitTime);

	/*
	 * HitScan
	 */
	FServerSideRewindResult ServerSideRewind(
	class ABlastCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize& HitLocation,
	float HitTime
	);

	FServerSideRewindResult ConfirmHit(
	const FFramePackage& Package,
	ABlastCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize& HitLocation
	);
	/*
	 * Projectile
	 */
	FServerSideRewindResult ProjectileServerSideRewind(
	ABlastCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize100& InitialVelocity,
	float HitTime
	);

	FServerSideRewindResult ProjectileConfirmHit(
	const FFramePackage& Package,
	ABlastCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize100& InitialVelocity,
	float HitTime
	);
	/*
	 * Shotgun
	 */
	FShotgunServerSideRewindResult ShotgunServerSideRewind(
	const TArray<ABlastCharacter*>& HitCharacters,
	const FVector_NetQuantize& TraceStart,
	const TArray<FVector_NetQuantize>& HitLocations,
	float HitTime
	);

	FShotgunServerSideRewindResult ShotGunConfirmHit(
	const TArray<FFramePackage>& FramePackages,
	const FVector_NetQuantize& TraceStart,
	const TArray<FVector_NetQuantize>& HitLocations
	);
private:
	UPROPERTY()
	ABlastCharacter* Character;

	UPROPERTY()
	class ABlasterPlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;
	
public:	

		
};
