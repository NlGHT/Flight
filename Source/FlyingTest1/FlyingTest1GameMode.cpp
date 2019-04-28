// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FlyingTest1GameMode.h"
#include "FlyingTest1Pawn.h"

AFlyingTest1GameMode::AFlyingTest1GameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AFlyingTest1Pawn::StaticClass();
}
