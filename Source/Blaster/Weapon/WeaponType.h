#pragma once
#define TRACE_LEGTH 800000.f
#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 251
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_MAX UMETA(DispalyName = "DefaultMAX")
};