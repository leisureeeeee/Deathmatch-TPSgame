// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"

#include "Weapon.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "Blaster/Charcacter/BlastCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;

}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
	/*FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithChannel = true;
	PathParams.bTraceWithCollision = true;
	PathParams.DrawDebugTime = 5.f;
	PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	PathParams.LaunchVelocity = GetActorForwardVector() * InitialSpeed;
	PathParams.MaxSimTime = 4.f;
	PathParams.ProjectileRadius = 5.f;
	PathParams.SimFrequency = 30.f;
	PathParams.StartLocation = GetActorLocation();
	PathParams.TraceChannel = ECC_Visibility;
	PathParams.ActorsToIgnore.Add(this);
	
	FPredictProjectilePathResult PathResult;
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);*/
}
#if WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	FName PropertyName = PropertyChangedEvent.Property != nullptr ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitialSpeed))
	{
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->InitialSpeed = InitialSpeed;
			ProjectileMovementComponent->MaxSpeed = InitialSpeed;
		}
	}
}
#endif
void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ABlastCharacter* OwnerCharacter = Cast<ABlastCharacter>(GetOwner());
	if (OwnerCharacter) {
		ABlasterPlayerController* OwnerController = Cast<ABlasterPlayerController>(OwnerCharacter->Controller);
		if (OwnerController) {
			const bool bHeadShot = Hit.BoneName.ToString() == FString("head");
			float RealDamage = bHeadShot ? Damage * HeadShotDamageCoe : Damage;
			
			if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
			{
				UGameplayStatics::ApplyDamage(OtherActor, RealDamage, OwnerController, this, UDamageType::StaticClass());
				Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
				return;
			}  
			ABlastCharacter* HitCharacter = Cast<ABlastCharacter>(OtherActor);
			if (bUseServerSideRewind && OwnerCharacter->GetLagCompensation() && OwnerCharacter->IsLocallyControlled() && HitCharacter)
			{
				OwnerCharacter->GetLagCompensation()->ProjectileServerScoreRequst(
					HitCharacter,
					TraceStart,
					InitialVelocity,
					OwnerController->GetServerTime() - OwnerController->SingleTripTime,
					OwnerCharacter->GetEquippedWeapon()
					);
			}
		}
	}
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
