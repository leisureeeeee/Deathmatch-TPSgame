// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Charcacter/BlastCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlastCharacter>(OwnerPawn) : BlasterOwnerCharacter;
	BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
	if (MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransfom = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransfom.GetLocation();
		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);
		UWorld* World = GetWorld();
		if (World)
		{
			if (FireHit.bBlockingHit)
			{
				ABlastCharacter* BlastCharacter = Cast<ABlastCharacter>(FireHit.GetActor());
					if (BlastCharacter &&InstigatorController)
					{
						const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");
						float RealDamage = Damage;
						if (bHeadShot)
						{
							RealDamage = Damage * HeadShotDamageCoe;
						}
						if (bUseServerSideRewind)
						{
							if (HasAuthority() && BlasterOwnerCharacter->IsLocallyControlled())
							{
								UE_LOG(LogTemp, Warning, TEXT("ApplyDamage"));
								UGameplayStatics::ApplyDamage(
								BlastCharacter,
								RealDamage,
								InstigatorController,
								this,
								UDamageType::StaticClass()
								);
							}
							else if(!HasAuthority() && BlasterOwnerCharacter->IsLocallyControlled())
							{
								UE_LOG(LogTemp, Warning, TEXT("ServerScoreRequest"));
								BlasterOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
									BlastCharacter,
									Start,
									HitTarget,
									BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime,
									this
								);
							}
						}
						else
						{
							if (HasAuthority())
							{
								UE_LOG(LogTemp, Warning, TEXT("ApplyDamage"));
								UGameplayStatics::ApplyDamage(
								BlastCharacter,
								RealDamage,
								InstigatorController,
								this,
								UDamageType::StaticClass()
								);
							}
						}
					}
			}
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				FireSound,
				GetActorLocation()
			);
		}
	}
}


void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	AController* InstigatorController = OwnerPawn->GetController();

	if (World)
	{
		FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;
		bool bHitBlock =  World->LineTraceSingleByChannel(
				OutHit,
				TraceStart,
				End,
				ECC_Visibility
			);
		if (OutHit.bBlockingHit)
		{
			ABlastCharacter* BlastCharacter = Cast<ABlastCharacter>(OutHit.GetActor());
			if (ImapctParticles)
			{

				UGameplayStatics::SpawnEmitterAtLocation(
					World,
					ImapctParticles,
					OutHit.ImpactPoint,
					OutHit.ImpactNormal.Rotation()
				);
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					HitSound,
					OutHit.ImpactPoint
				);
			}
			ClientParticles(World, OutHit);
		}
	}
}

void AHitScanWeapon::ClientParticles_Implementation(UWorld* World, FHitResult FireHit)
{
	if (ImapctParticles)
	{

		UGameplayStatics::SpawnEmitterAtLocation(
			World,
			ImapctParticles,
			FireHit.ImpactPoint,
			FireHit.ImpactNormal.Rotation()
		);
	}
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			FireHit.ImpactPoint
		);
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			FireSound,
			GetActorLocation()
		);
	}
}
