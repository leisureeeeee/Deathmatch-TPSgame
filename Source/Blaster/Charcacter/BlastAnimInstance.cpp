// Fill out your copyright notice in the Description page of Project Settings.


#include "BlastAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Weapon/Weapon.h"
#include "Blaster/BlasterType/CombatState.h"

void UBlastAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlastCharacter = Cast<ABlastCharacter>(TryGetPawnOwner());
}

void UBlastAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (BlastCharacter == nullptr) {
		BlastCharacter = Cast<ABlastCharacter>(TryGetPawnOwner());
	}
	if (BlastCharacter == nullptr) return;

	FVector Velocity = BlastCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = BlastCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BlastCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = BlastCharacter->IsWeaponEquipped();
	EquippedWeapon = BlastCharacter->GetEquippedWeapon();
	bIsCrouched = BlastCharacter->bIsCrouched;
	bIsAiming = BlastCharacter->IsAiming();
	TurningInPlace = BlastCharacter->GetTurningInPlace();
	bRotateRootBone = BlastCharacter->ShouldRotateRootBone();
	bElimmed = BlastCharacter->IsElimmed();
	FRotator AimRotation = BlastCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlastCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 5.f);
	YawOffset = DeltaRotation.Yaw;
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlastCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);    

	AO_Yaw = BlastCharacter->GetAO_Yaw();
	AO_Pitch = BlastCharacter->GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlastCharacter->GetMesh()) {
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BlastCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (BlastCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlastCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
		}

	}
	bUseFABRIK = BlastCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	if (BlastCharacter->IsLocallyControlled())
	{
		bUseFABRIK = !BlastCharacter->IsLocallyControlled();
	}
	bUseAimOffsets = BlastCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bTransformRightHand = BlastCharacter->GetCombatState() != ECombatState::ECS_Reloading;

}
