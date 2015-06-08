// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UE4TopDownCamera.h"
#include "TDCAIController.h"

ATDCAIController::ATDCAIController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->bAttachToPawn = true;
}

void ATDCAIController::Possess(class APawn* inPawn)
{
	Super::Possess(inPawn);

	SetActorTickEnabled(true);
}

void ATDCAIController::UnPossess()
{
	/*
	if (GetPawn())
	{
		const APawn* TargetPawn = Cast<APawn>(CurrentTarget);
		if (TargetPawn != NULL)
		{
			AStrategyAIController* const AITarget = Cast<AStrategyAIController>(TargetPawn->Controller);
			if (AITarget != NULL)
			{
				AITarget->UnClaimAsTarget(this);
			}
		}
	}*/

	SetActorTickEnabled(false);
	Super::UnPossess();
}