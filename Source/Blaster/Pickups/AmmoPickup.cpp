// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickup.h"
#include "Blaster/Charcacter/BlastCharacter.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	ABlastCharacter* BlastCharacter = Cast<ABlastCharacter>(OtherActor);
	if (BlastCharacter)
	{
		UCombatComponent* Combat = BlastCharacter->GetCombat();
		if (Combat)
		{
			Combat->PickupAmmo(WeaponType, AmmoAmount);
		}
	}
	Destroy();
}
