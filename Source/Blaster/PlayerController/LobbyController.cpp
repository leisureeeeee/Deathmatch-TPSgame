// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyController.h"

#include "Blaster/HUD/LobbyButton.h"
#include "Blaster/HUD/LobbyHUD.h"

void ALobbyController::SetHUDLobbyMenu()
{
	LobbyHUD = LobbyHUD == nullptr ? Cast<ALobbyHUD>(GetHUD()) : LobbyHUD;
	LobbyHUD->AddLobbyMenu();
	if (LobbyHUD->LobbyMenu && LobbyHUD->LobbyMenu->StartGameButton)
	{
		LobbyHUD->LobbyMenu->MenuSetup();
	}
}
