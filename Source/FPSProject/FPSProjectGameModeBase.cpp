// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "FPSProjectGameModeBase.h"
#include "Engine.h"

void AFPSProjectGameModeBase::StartPlay() {
	Super::StartPlay();

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Welcome to this FPSGameMode!"));
	}
}
