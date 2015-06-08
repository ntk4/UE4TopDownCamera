// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "TDCCharacter.generated.h"

#define CHARACTER_MESH TEXT("/Game/TopDownBP/Character/TopDownSkeletalMesh")
#define CHARACTER_ANIMATION TEXT("/Game/TopDownBP/Character/TopDownAnimBlueprint")

UCLASS()
class UE4TOPDOWNCAMERA_API ATDCCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/* Called every frame */
	virtual void Tick(float DeltaSeconds) override;

	/* Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	/************************************************************************/
	/* Movement                                                             */
	/************************************************************************/

	virtual void MoveForward(float Val);

	virtual void MoveRight(float Val);
};
