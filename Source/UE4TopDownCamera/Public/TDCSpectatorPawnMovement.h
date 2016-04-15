// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/SpectatorPawnMovement.h"
#include "TDCSpectatorPawnMovement.generated.h"

/**
 * 
 */
UCLASS()
class UE4TOPDOWNCAMERA_API UTDCSpectatorPawnMovement : public USpectatorPawnMovement
{
	GENERATED_BODY()

public:
	
	UTDCSpectatorPawnMovement(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

private:
	bool bInitialLocationSet;
};