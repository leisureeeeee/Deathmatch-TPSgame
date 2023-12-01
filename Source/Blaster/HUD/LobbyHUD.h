// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/HUD.h"
#include "LobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ALobbyHUD : public AHUD
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Lobby")
	TSubclassOf<class UUserWidget> LobbyMenuClass;
	
	UPROPERTY()
	class ULobbyButton* LobbyMenu;

	void AddLobbyMenu();
	
	
};
