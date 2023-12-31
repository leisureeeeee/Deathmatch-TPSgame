// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffComponent();
	friend class ABlastCharacter;
	void Heal(float HealAmount, float HealingTime);
	void ReplenishShield(float ShieldAmount, float ReplenishTime);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);

protected:
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
	void ShieldRampUp(float DeltaTime);
private:
	UPROPERTY()
	class ABlastCharacter* Character;

	bool bHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;
	
	bool bReplenishShield = false;
	float ShieldReplenishRate = 0.f;
	float ShieldRepenishAmount = 0.f;
	
	FTimerHandle SpeedBuffTimer;
	void ResetSpeeds();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);
};
