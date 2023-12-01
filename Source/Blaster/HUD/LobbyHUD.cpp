// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyHUD.h"

void ALobbyHUD::AddLobbyMenu()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && LobbyMenuClass) {
		LobbyMenu = CreateWidget<ULobbyButton>(PlayerController, LobbyMenuClass);
	}
}