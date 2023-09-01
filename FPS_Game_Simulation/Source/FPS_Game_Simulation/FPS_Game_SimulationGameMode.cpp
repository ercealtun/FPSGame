// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_Game_SimulationGameMode.h"
#include "FPS_Game_SimulationCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPS_Game_SimulationGameMode::AFPS_Game_SimulationGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
