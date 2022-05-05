// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetGameMode.h"
#include "NetCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANetGameMode::ANetGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
