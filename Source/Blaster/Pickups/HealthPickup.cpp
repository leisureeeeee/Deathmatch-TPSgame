// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Blaster/Charcacter/BlastCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
AHealthPickup::AHealthPickup()
{
	bReplicates = true;

}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	ABlastCharacter* BlastCharacter = Cast<ABlastCharacter>(OtherActor);
	if (BlastCharacter)
	{
		UBuffComponent* Buff = BlastCharacter->GetBuff();
		if(Buff)
		{
			Buff->Heal(HealAmount, HealingTime);
		}
	}
	Destroy();
}

