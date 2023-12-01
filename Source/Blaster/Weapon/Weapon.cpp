// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Casing.h"
#include "Kismet/KismetMathLibrary.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
	RecoilTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RecoilTimeline"));
	
}

void AWeapon::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

void AWeapon::Recoil()
{
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlastCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if (BlasterOwnerCharacter)
	{
		float YValue = 0;
		YValue = FMath::FInterpTo(YValue, YValue + 1, FireDelay, 1.f);
		BlasterOwnerCharacter->AddControllerPitchInput(-YValue);
	}
}

void AWeapon::BeginPlay()
{	
	Super::BeginPlay();
	if (PickupWidget) {
		PickupWidget->SetVisibility(false);
	}
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndlap);

	UpdateFunctionFloat.BindDynamic(this, &AWeapon::UpdateRecoilTimeLine);
	if (RecoilTimeFloatCurve)
	{
		RecoilTimeline->AddInterpFloat(RecoilTimeFloatCurve, UpdateFunctionFloat);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME_CONDITION(AWeapon, bUseServerSideRewind, COND_OwnerOnly);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlastCharacter* BlasterCharacter = Cast<ABlastCharacter>(OtherActor);
	if (BlasterCharacter) {
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlastCharacter* BlasterCharacter = Cast<ABlastCharacter>(OtherActor);
	if (BlasterCharacter) {
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::OnPingTooHigh(bool bPingTooHigh)
{
	UE_LOG(LogTemp, Warning, TEXT("OnPingTooHigh"));
	bUseServerSideRewind = !bPingTooHigh;
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		/*if (WeaponType == EWeaponType::EWT_SubmachineGun)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToChannels(ECR_Ignore);
		}*/
		EnableCustomDepth(false);
		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlastCharacter>(GetOwner()) : BlasterOwnerCharacter;
		if (BlasterOwnerCharacter && bUseServerSideRewind)
		{
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
			if (BlasterOwnerController && HasAuthority() && BlasterOwnerController->HighPingDelegate.IsBound())
			{
				BlasterOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
			}
		}
		break;
	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
		WeaponMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
}


void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo -1, 0, MagCapacity);
	SetHUDAmmo();
	if (HasAuthority())
	{
		ClientUpdateAmmo(Ammo);
	}
	else
	{
		Sequence++;
	}
}
void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
	ClientAddAmmo(AmmoToAdd);
}

void AWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	if (HasAuthority()) return;
	Ammo = ServerAmmo;
	Sequence--;
	Ammo -= Sequence;
	SetHUDAmmo();
}

void AWeapon::ClientAddAmmo_Implementation(int32 AmmoToAdd)
{
	if (HasAuthority()) return;
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	/*BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlastCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if (BlasterOwnerCharacter && BlasterOwnerCharacter->GetCombat() && IsFull())*/
	SetHUDAmmo();
}

void AWeapon::SetHUDAmmo()
{
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlastCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if (BlasterOwnerCharacter)
	{
		BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
		if (BlasterOwnerController)
		{
			BlasterOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}


void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		BlasterOwnerCharacter = nullptr;
		BlasterOwnerController = nullptr;
	}
	else
	{
		SetHUDAmmo();
	}
	
}

void AWeapon::UpdateRecoilTimeLine(float Output)
{
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlastCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if (BlasterOwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateRecoilTimeLine:%f"), Output);
		BlasterOwnerCharacter->AddControllerYawInput(Output);
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{

	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		/*if (WeaponType == EWeaponType::EWT_SubmachineGun)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToChannels(ECR_Ignore);
		}*/
		EnableCustomDepth(false);
		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlastCharacter>(GetOwner()) : BlasterOwnerCharacter;
		if (BlasterOwnerCharacter && bUseServerSideRewind)
		{
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
			if (BlasterOwnerController && HasAuthority() && !BlasterOwnerController->HighPingDelegate.IsBound())
			{
				UE_LOG(LogTemp, Warning, TEXT("HighPingDelegate"));
				BlasterOwnerController->HighPingDelegate.AddDynamic(this, &AWeapon::OnPingTooHigh);
			}
		}
		break;
	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
		WeaponMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);
		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlastCharacter>(GetOwner()) : BlasterOwnerCharacter;
		if (BlasterOwnerCharacter && bUseServerSideRewind)
		{
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
			if (BlasterOwnerController && HasAuthority() && BlasterOwnerController->HighPingDelegate.IsBound())
			{
				BlasterOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
			}
		}
		break;
	}
}

bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget) {
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation) {
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CasingClass) {
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket) {
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);

			UWorld* World = GetWorld();
			if (World) {
				World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}
		SpendRound();
}

void AWeapon::Dropped()
{
	if (HasAuthority())

	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterOwnerController = nullptr;
}


FVector AWeapon::TraceEndWithScatter(const FVector& HitTarget)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return FVector();
	FTransform SocketTransfom = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	FVector TraceStart = SocketTransfom.GetLocation() + 10.f;
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	FVector EndLoc = SphereCenter + RandVec;
	FVector ToEndLoc = EndLoc - TraceStart;
	return FVector(TraceStart + ToEndLoc * TRACE_LEGTH / ToEndLoc.Size());
	
}

