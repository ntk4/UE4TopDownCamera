// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

//////////////////////////////////////////////////////////////////////////

#pragma once

#include "TDCCameraComponent.h"
#include "TDCSpectatorPawn.generated.h"

//@TODO: Write a comment here
UCLASS(Blueprintable, BlueprintType)
class UE4TOPDOWNCAMERA_API ATDCSpectatorPawn : public ASpectatorPawn
{
	GENERATED_UCLASS_BODY()

private:

	// The camera component for this camera
	UPROPERTY(Category = CameraActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTDCCameraComponent* CameraComponent;

	UPROPERTY(Category = CameraActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoomComp;

	UPROPERTY(Category = CameraActor, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bFollowMainCharacter;

public:

	void MoveForward(float Val) override;

	/** Handles the mouse scrolling down. */
	void OnMouseScrollUp();

	/** Handles the mouse scrolling up. */
	void OnMouseScrollDown();
	
	/* Returns a pointer to the strategy camera component the pawn has. */
	UTDCCameraComponent* GetCameraComponent();

	FORCEINLINE bool GetFollowMainCharacter() { return bFollowMainCharacter; };

	FORCEINLINE void SetFollowMainCharacter(bool Val) { bFollowMainCharacter = Val; };
};


