// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "TDCCharacter.h"
#include "Camera.h"
#include "TDCAIController.h"
#include "TDCPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UE4TOPDOWNCAMERA_API ATDCPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

	/* Spawns the main character of the game at the position of the first APlayerStart actor found in the world */
	void SpawnMainCharacter();

	/* The main character of the game. It's spawned and maintained by the player controller */
	ATDCCharacter* MainCharacter = nullptr;

	/* The AI Controller used for the main character */
	ATDCAIController* MainCharacterController = nullptr;

	// true when the move command has been issued
	bool bMoveToMouseCursor;

	// true when we suspect a move command that can be invalidated by a swipe, that signals a camera only scroll
	bool bCandidateMoveToMouseCursor;

	void MoveToMouseCursor();

	void MoveToTouchLocationPressed(const ETouchIndex::Type FingerIndex, const FVector Location);

	void MoveToTouchLocationReleased(const ETouchIndex::Type FingerIndex, const FVector Location);

	void OnSetDestinationPressed();

	void OnSetDestinationReleased();

	void OnMouseScrollUp();

	void OnMouseScrollDown();

	void MoveForward(float Val);

	void MoveRight(float Val);

	void MoveMainCharacterToLocation(FVector& location);

	virtual void UpdateRotation(float DeltaTime) override;

	virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;

protected:
	/** if set, input and camera updates will be ignored */
	uint8 bIgnoreInput : 1;

	/** helper function to toggle input detection. */
	void SetIgnoreInput(bool bIgnore);

	/** currently selected actor */
	TWeakObjectPtr<AActor> SelectedActor;

	/** Swipe anchor. */
	FVector SwipeAnchor3D;

	FVector2D PrevSwipeScreenPosition;

	/** Previous swipe mid point. */
	FVector2D PrevSwipeMidPoint;

	/** Custom input handler. */
	UPROPERTY()
	class UTDCInput* InputHandler;

	/** set desired camera position. */
	void SetCameraTarget(const FVector& CameraTarget);

	/** Input handlers. */
	void OnTapPressed(const FVector2D& ScreenPosition, float DownTime);
	void OnHoldPressed(const FVector2D& ScreenPosition, float DownTime);
	void OnHoldReleased(const FVector2D& ScreenPosition, float DownTime);
	void OnSwipeStarted(const FVector2D& AnchorPosition, float DownTime);
	void OnSwipeUpdate(const FVector2D& ScreenPosition, float DownTime);
	void OnSwipeReleased(const FVector2D& ScreenPosition, float DownTime);
	void OnSwipeTwoPointsStarted(const FVector2D& ScreenPosition1, const FVector2D& ScreenPosition2, float DownTime);
	void OnSwipeTwoPointsUpdate(const FVector2D& ScreenPosition1, const FVector2D& ScreenPosition2, float DownTime);
	void OnPinchStarted(const FVector2D& AnchorPosition1, const FVector2D& AnchorPosition2, float DownTime);
	void OnPinchUpdate(const FVector2D& ScreenPosition1, const FVector2D& ScreenPosition2, float DownTime);

	/**
	* Change current selection (on toggle on the same).
	*
	* @param	NewFocus	Actor to focus on.
	* @param	NewPosition
	*/
	void SetSelectedActor(AActor* NewFocus, const FVector& NewPosition);

	/**
	* Get friendly target under screen space coordinates.
	*
	* @param	ScreenPoint	Screen coordinates to check
	* @param	WorldPoint	Point in the world the screen coordinates projected onto.
	*/
	AActor* GetFriendlyTarget(const FVector2D& ScreenPoint, FVector& WorldPoint) const;

public:

	void BeginPlay() override;

	void PlayerTick(float DeltaTime);

	void SetupInputComponent();

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Burnt Dragon")
	float MinDistanceToMoveCharacter;

	UFUNCTION(BluePrintCallable, Category = "Burnt Dragon")
	void SetNewMoveDestination(FVector DestLocation);

	/** Helper to return cast version of Spectator pawn. */
	class ATDCSpectatorPawn* GetSpectatorPawn() const;

	/** Helper to return camera component via spectator pawn. */
	class UTDCCameraComponent* GetCameraComponent() const;
};
