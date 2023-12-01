// Fill out your copyright notice in the Description page of Project Settings.


#include "BlastCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "../BlasterComponents/CombatComponent.h"
#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlastAnimInstance.h"
#include "../Blaster.h"
#include "../PlayerController/BlasterPlayerController.h"
#include "../GameMode/BlasterGameMode.h"
#include "TimerManager.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "Blaster/Weapon/WeaponType.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
ABlastCharacter::ABlastCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600;

	FollowCamera2 = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera2->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	CameraBoom->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);
	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);
	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	TurningInPlace = ETurningInPlace::ETIP_NotTuring;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("blanket"));
	blanket->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("blanket"), blanket);

	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("backpack"), backpack);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ABlastCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlastCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlastCharacter, Health);
	DOREPLIFETIME(ABlastCharacter, Shield);
}


void ABlastCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlastCharacter::BeginPlay()
{	
	Super::BeginPlay();
	SpawnDefaultWeapon();
	if (HasAuthority()) {
		OnTakeAnyDamage.AddDynamic(this, &ABlastCharacter::ReceiveDamage);
	}
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
}

void ABlastCharacter::Destroyed()
{
	Super::Destroyed();
	ABlasterGameMode* BlasterGameState = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = BlasterGameState && BlasterGameState->GetMatchState() != MatchState::InProgress;
	
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void ABlastCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController) {
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ABlastCharacter::UpdateHUDShield()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController) {
		BlasterPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void ABlastCharacter::UpdateHUDAmmo()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController && Combat && Combat->EquippedWeapon) {
		BlasterPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		BlasterPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

void ABlastCharacter::SpawnDefaultWeapon()
{
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (BlasterGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon =  World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

void ABlastCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		 if (BlasterPlayerState)
		 {
			 BlasterPlayerState->AddToScore(0.f);
		 	BlasterPlayerState->AddToDefeats(0);
		 }
	}
}

void ABlastCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled()) {
		AimOffset(DeltaTime);

	}
	else {
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f) {
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
	HideCameraIfCharacterClose();
	PollInit();
	if (GetCharacterMovement())
	{
		if (GetCharacterMovement()->IsFalling() && IsLocallyControlled())
		{
			FallingTime += DeltaTime;
			if (FallingTime >= 5.f)
			{
				BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller):BlasterPlayerController;
				ServerFallingElim(BlasterPlayerController);
				FallingTime = 0.f;

			}
		}
	}
}



void ABlastCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlastCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlastCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlastCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlastCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlastCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlastCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlastCharacter::FireButtionReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABlastCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABlastCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABlastCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABlastCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABlastCharacter::LookUp);

}

void ABlastCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat) {
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed,
			GetCharacterMovement()->MaxWalkSpeedCrouched);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ABlasterPlayerController>(Controller);
		}
	}
}

void ABlastCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage) {
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlastCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage) {
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABlastCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage) {
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Rifle");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlastCharacter::Elim()
{
	if (Combat && Combat->EquippedWeapon)
	{
		if (Combat->EquippedWeapon->bDestroyWeapon)
		{
			Combat->EquippedWeapon->Destroy();
		}
		else
		{
			Combat->EquippedWeapon->Dropped();
		}
	} 
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ABlastCharacter::ElimTimerFinished,
		ElimDelay
	);

}
void ABlastCharacter::ElimTimerFinished()
{
	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (BlasterGameMode) {
		BlasterGameMode->RequestRespawn(this, Controller);
	}
}

void ABlastCharacter::MulticastElim_Implementation()
{
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}
	bElimmed = true;
	PlayElimMontage();
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (BlasterPlayerController)
	{
		DisableInput(BlasterPlayerController);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABlastCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage) {
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlastCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamegeType, AController* InstigatorController, AActor* DamageCauser)
{
	if (bElimmed) return;
	float DamageToHealth = Damage;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
			Shield = 0.f;
		}
	}
	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);
	UpdateHUDHealth();
	UpdateHUDShield();

	PlayHitReactMontage();

	if (Health == 0.f) {
		ABlasterGameMode* BlasterGameMode =  GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if (BlasterGameMode) {
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller):BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
	
}


void ABlastCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f) {
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void ABlastCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f) {
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void ABlastCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABlastCharacter::LookUp(float Value)
{	

	AddControllerPitchInput(Value);

}

void ABlastCharacter::EquipButtonPressed()
{
	if (Combat) {
		if (HasAuthority()) {
		Combat->EquipWeapon(OverlappingWeapon);
		}
		else {
			ServerEquipButtonPressed();
		}
	}
}

void ABlastCharacter::CrouchButtonPressed()
{	
	if (bIsCrouched) {
		UnCrouch();
	}
	else {
		Crouch();
	}
}

void ABlastCharacter::ReloadButtonPressed()
{
	if(Combat)
	{
		Combat->Reload();
	}
}

void ABlastCharacter::AimButtonPressed()
{
	if (Combat) {
		Combat->SetAiming(true);
	}
}

void ABlastCharacter::AimButtonReleased()
{
	if (Combat) {
		Combat->SetAiming(false);
	}
}

void ABlastCharacter::FireButtonPressed()
{
	if (Combat) {
		Combat->FireButtonPressed(true);
	}
}

void ABlastCharacter::FireButtionReleased()
{
	if (Combat) {
		Combat->FireButtonPressed(false);
	}
}

void ABlastCharacter::AimOffset(float DeltaTime)
{	
	if (Combat && Combat->EquippedWeapon == nullptr) return;

	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if (Speed == 0.f && !bIsInAir) {
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTuring) {
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) {
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTuring;
	}

	CalculateAO_Pitch();
}

void ABlastCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled()) {
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlastCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f) {
		TurningInPlace = ETurningInPlace::ETIP_NotTuring;
		return;
	}
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;
	if (FMath::Abs(ProxyYaw) > TurnThershold) {
		if (ProxyYaw > TurnThershold) {
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThershold) {
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else {
			TurningInPlace = ETurningInPlace::ETIP_NotTuring;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTuring;
}

void ABlastCharacter::Jump()
{
	if (bIsCrouched) {
		UnCrouch();
	}
	else
	{
		{
			Super::Jump();
		}
	}
}

void ABlastCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat) {
		Combat->EquipWeapon(OverlappingWeapon);
	}
}


void ABlastCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTuring) {
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f) {
			TurningInPlace = ETurningInPlace::ETIP_NotTuring;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);

		}
	}
}


void ABlastCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera2->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold) {
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh()) {
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else {
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh()) {
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

float ABlastCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
	
}

void ABlastCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
		PlayHitReactMontage();
	
}

void ABlastCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
}


void ABlastCharacter::UpdateDissolveMeterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ABlastCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlastCharacter::UpdateDissolveMeterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABlastCharacter::ServerFallingElim_Implementation(ABlasterPlayerController* PlayerController)
{
	ABlasterGameMode* BlasterGameMode =  GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (BlasterGameMode) {
		BlasterGameMode->PlayerEliminated(this, PlayerController, PlayerController);
	}
}

void ABlastCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{	
	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon; 
	if (IsLocallyControlled()) {
		if (OverlappingWeapon) {
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ABlastCharacter::IsWeaponEquipped()
{	

	return (Combat && Combat->EquippedWeapon);
}

bool ABlastCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* ABlastCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

void ABlastCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon) {
		LastWeapon->ShowPickupWidget(false);
	}
}
FVector ABlastCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState ABlastCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

bool ABlastCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}






