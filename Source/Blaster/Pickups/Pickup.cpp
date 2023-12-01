// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Blaster/Weapon/WeaponType.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapSphere->AddLocalOffset(FVector(0.f, 0.f, 85.f));
	PickUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickUpMesh->SetupAttachment(OverlapSphere);
	PickUpMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickUpMesh->SetRelativeScale3D(FVector(5.f,5.f, 5.f));
	PickUpMesh->SetRenderCustomDepth(true);
	PickUpMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
	PickupEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickupEffectComponent"));
	PickupEffectComponent->SetupAttachment(RootComponent);
}
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PickUpMesh)
	{
		PickUpMesh->AddLocalRotation(FRotator(0.f, BaseTurnRate* DeltaTime, 0.f));
	}
}

void APickup::Destroyed()
{
	Super::Destroyed();
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PickupSound,
			GetActorLocation()
		);
	}
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickupEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}
}

void APickup::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			BindOverlapTimer,
			this,
			&APickup::BindOverlapTimerFinished,
			BindOverlapTime
		);
	}
	
}

void APickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void APickup::BindOverlapTimerFinished()
{
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this,&APickup::OnSphereOverlap);
}


