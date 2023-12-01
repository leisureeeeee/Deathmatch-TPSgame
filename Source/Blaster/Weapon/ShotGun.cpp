// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"

#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


void AShotGun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket && InstigatorController)
	{
		const FTransform SocketTransfom = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransfom.GetLocation();
		TMap<ABlastCharacter*, uint32> HitMap;
		TMap<ABlastCharacter*, uint32> HitHeadMap;
		for (auto HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start,HitTarget, FireHit);
			ABlastCharacter* BlastCharacter = Cast<ABlastCharacter>(FireHit.GetActor());
			if (BlastCharacter &&InstigatorController && BlastCharacter->Controller != InstigatorController)
			{
				if (FireHit.BoneName.ToString() == FString("head"))
				{
					if (HitHeadMap.Contains(BlastCharacter))
					{
						HitHeadMap[BlastCharacter]++;
					}
					else
					{
						HitHeadMap.Emplace(BlastCharacter, 1);
					}
				}
				else
				{
					if (HitMap.Contains(BlastCharacter))
					{
						HitMap[BlastCharacter]++;
					}
					else
					{
						HitMap.Emplace(BlastCharacter, 1);
					}
				}
			}
		}
		TArray<ABlastCharacter*> HitCharacters;
		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlastCharacter>(OwnerPawn) : BlasterOwnerCharacter;
		BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key  &&InstigatorController)
			{
				if (HasAuthority())
				{
					if (BlasterOwnerCharacter->IsLocallyControlled())
					{
						UE_LOG(LogTemp, Warning, TEXT("HitMap:%f"), Damage * HitPair.Value * GetHeadShotDamageCoe())
						UE_LOG(LogTemp, Warning, TEXT("ApplyDamage"))
						UGameplayStatics::ApplyDamage(
						HitPair.Key,
						Damage * HitPair.Value,
						InstigatorController,
						this,
						UDamageType::StaticClass()
						);
					}
					else if (!bUseServerSideRewind)
					{
						UE_LOG(LogTemp, Warning, TEXT("ApplyDamage"))
						UGameplayStatics::ApplyDamage(
						HitPair.Key,
						Damage * HitPair.Value,
						InstigatorController,
						this,
						UDamageType::StaticClass()
						);
					}
				}
				HitCharacters.Add(HitPair.Key);
			}
		}
		for (auto HitPair : HitHeadMap)
		{
			if (HitPair.Key  &&InstigatorController)
			{
				if (HasAuthority())
				{
					if (BlasterOwnerCharacter->IsLocallyControlled())
					{
						UE_LOG(LogTemp, Warning, TEXT("HitHeadMap:%f"), Damage * HitPair.Value * GetHeadShotDamageCoe())
						UGameplayStatics::ApplyDamage(
						HitPair.Key,
						Damage * HitPair.Value * GetHeadShotDamageCoe(),
						InstigatorController,
						this,
						UDamageType::StaticClass()
						);
					}
					else if (!bUseServerSideRewind)
					{
						UGameplayStatics::ApplyDamage(
						HitPair.Key,
						Damage * HitPair.Value * GetHeadShotDamageCoe(),
						InstigatorController,
						this,
						UDamageType::StaticClass()
						);
					}
				}
				HitCharacters.Add(HitPair.Key);
			}
		}
		if (!HasAuthority() && bUseServerSideRewind)
		{
			if (BlasterOwnerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensation() && BlasterOwnerCharacter->IsLocallyControlled())
			{
				UE_LOG(LogTemp, Warning, TEXT("ServerShotgunServerScoreRequest"))
				BlasterOwnerCharacter->GetLagCompensation()->ServerShotgunServerScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime,
					this
				);
			}
		}
	}
}

void AShotGun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return;
	
	const FTransform SocketTransfom = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransfom.GetLocation();
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	
	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		FVector ToEndLoc = EndLoc - TraceStart;
		ToEndLoc = TraceStart + ToEndLoc * TRACE_LEGTH / ToEndLoc.Size();

		HitTargets.Add(ToEndLoc);
	}
}
