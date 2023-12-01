

#include "PickupSpawnPoint.h"

#include "Pickup.h"

APickupSpawnPoint::APickupSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void APickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnPickupTimer((AActor*)nullptr);
}

void APickupSpawnPoint::SpawnPickup()
{
	int32 NumPickupClasses = PickupClasses.Num();
	if (NumPickupClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, NumPickupClasses - 1);
		SpawnedPickup = GetWorld()->SpawnActor<APickup>(PickupClasses[Selection], GetActorTransform());
		 if (HasAuthority() && SpawnedPickup)
		 {
			 SpawnedPickup->OnDestroyed.AddDynamic(this, &APickupSpawnPoint::StartSpawnPickupTimer);
		 }
	}
}

void APickupSpawnPoint::SpawnPickuTimerFinished()
{
	if (HasAuthority())
	{
		SpawnPickup();
	}	
}

void APickupSpawnPoint::StartSpawnPickupTimer(AActor* DestroyActor)
{
	const float SpawnTime = FMath::FRandRange(SpawnPickupTimeMin, SpawnPickupTimeMax);
	GetWorldTimerManager().SetTimer(
		SpawnPickupTimer,
		this,
		&APickupSpawnPoint::SpawnPickuTimerFinished,
		SpawnTime
	);
}

void APickupSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

