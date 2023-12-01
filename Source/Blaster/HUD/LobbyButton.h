// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyButton.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ULobbyButton : public UUserWidget
{
	GENERATED_BODY()
public:
	void MenuSetup();
	void MenuTearDown();
	UPROPERTY(meta = (BindWidget))
	class UButton* StartGameButton;
private:
	UFUNCTION()
	void StartGameButtonOnClick();
	UPROPERTY()
	class APlayerController* PlayerController;
};
