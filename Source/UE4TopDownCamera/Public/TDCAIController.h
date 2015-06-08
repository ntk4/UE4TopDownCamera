// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AIController.h"
#include "TDCAIController.generated.h"

/**
 * 
 */
UCLASS()
class UE4TOPDOWNCAMERA_API ATDCAIController : public AAIController
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Possess(class APawn* inPawn) override;
	virtual void UnPossess() override;

};
