// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyButton.h"

#include "Components/Button.h"

void ULobbyButton::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
	if (StartGameButton && !StartGameButton->OnClicked.IsBound())
	{
		StartGameButton->OnClicked.AddDynamic(this, &ULobbyButton::StartGameButtonOnClick);
	}
}

void ULobbyButton::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
	if (StartGameButton && StartGameButton->OnClicked.IsBound())
	{
		StartGameButton->OnClicked.RemoveDynamic(this, &ULobbyButton::StartGameButtonOnClick);
	}
}

void ULobbyButton::StartGameButtonOnClick()
{
	UWorld* World = GetWorld();
	if (World) {
		if (World->ServerTravel(FString("/Game/Maps/BlasterMap?Listen")))
		{
			MenuTearDown();
		}
	}
}
