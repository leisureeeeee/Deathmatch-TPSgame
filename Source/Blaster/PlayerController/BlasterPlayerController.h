// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountDown(float CountDownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void ReceivedPlayer() override;
	virtual float GetServerTime();
	void OnMatchStateSet(FName State);
	void HandleMatchHasStart();
	void HandleCooldown();
	void SetHUDLobbyMenu();
	float SingleTripTime = 0;
	FHighPingDelegate HighPingDelegate;
protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();
	virtual void SetupInputComponent() override;

	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f;
	UPROPERTY(EditAnywhere, Category=Time)
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckingPing(float DeltaTime);

	void ShowReturnToMainMenu();
private:
	UPROPERTY()
	class ABlastHUD* BlasterHUD;

	UPROPERTY(EditAnywhere, Category=HUD)
	TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;
	
	UPROPERTY()
	class UReturnToMainMenu* ReturnToMainMenu;

	bool bReturnToMainMenuOpen = false;

	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;
	float LevelStartTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;
	UFUNCTION()
	void OnRep_MatchState();

	class UCharacterOVerlay* CharacterOVerlay;
	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	bool bInitializeHealth = false;
	bool bInitializeScore = false;
	bool bInitializeDefeats = false;
	bool bInitializeShield = false;
	float HUDMaxHealth;
	float HUDShield;
	float HUDMaxShield;
	float HUDScore;
	int32 HUDDefeats;
	float HUDCarriedAmmo;
	float HUDWeaponAmmo;
	bool bInitializeCarriedAmmo = false;
	bool bInitializeWeaponAmmo =false;

	float HighPingRunningTime = 0.f;
	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.f;

	float PingAnimationRunningTime = 0.f;

	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);
	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;
};
