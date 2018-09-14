// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UE4TopDownCamera.h"
#include "TDCPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

ATDCPlayerController::ATDCPlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	bEnableMouseOverEvents = true;
	bEnableTouchEvents = true;
	bEnableTouchOverEvents = true;

	bShowMouseCursor = true;
	CurrentMouseCursor = EMouseCursor::Crosshairs;
	
	bMoveToMouseCursor = false;
	bCandidateMoveToMouseCursor = false;
	MinDistanceToMoveCharacter = 20.0f;
}

void ATDCPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();


	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ATDCPlayerController::OnMouseScrollUp);
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ATDCPlayerController::OnMouseScrollDown);

	InputComponent->BindAxis("MoveForward", this, &ATDCPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATDCPlayerController::MoveRight);

	InputComponent->BindAction("MoveToLocation", IE_Pressed, this, &ATDCPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("MoveToLocation", IE_Released, this, &ATDCPlayerController::OnSetDestinationReleased);
	//InputComponent->BindAction("ZoomAction", IE_Pressed, this, &ATDCPlayerController::ZoomActionStart);
	//InputComponent->BindAction("ZoomAction", IE_Released, this, &ATDCPlayerController::ZoomActionEnd);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATDCPlayerController::MoveToTouchLocationPressed);
	InputComponent->BindTouch(EInputEvent::IE_Released, this, &ATDCPlayerController::MoveToTouchLocationReleased);

	//InputComponent->BindAxis("ZoomAxis", this, &ATDCPlayerController::ZoomAxis);

	// Camera controls
	InputHandler = NewObject<UTDCInput>(this, UTDCInput::StaticClass(), TEXT("TDCInput"));

	BIND_1P_ACTION(InputHandler, EGameKey::Tap, IE_Pressed, &ATDCPlayerController::OnTapPressed);
	BIND_1P_ACTION(InputHandler, EGameKey::Hold, IE_Pressed, &ATDCPlayerController::OnHoldPressed);
	BIND_1P_ACTION(InputHandler, EGameKey::Hold, IE_Released, &ATDCPlayerController::OnHoldReleased);
	BIND_1P_ACTION(InputHandler, EGameKey::Swipe, IE_Pressed, &ATDCPlayerController::OnSwipeStarted);
	BIND_1P_ACTION(InputHandler, EGameKey::Swipe, IE_Repeat, &ATDCPlayerController::OnSwipeUpdate);
	BIND_1P_ACTION(InputHandler, EGameKey::Swipe, IE_Released, &ATDCPlayerController::OnSwipeReleased);
	BIND_2P_ACTION(InputHandler, EGameKey::SwipeTwoPoints, IE_Pressed, &ATDCPlayerController::OnSwipeTwoPointsStarted);
	BIND_2P_ACTION(InputHandler, EGameKey::SwipeTwoPoints, IE_Repeat, &ATDCPlayerController::OnSwipeTwoPointsUpdate);
	BIND_2P_ACTION(InputHandler, EGameKey::Pinch, IE_Pressed, &ATDCPlayerController::OnPinchStarted);
	BIND_2P_ACTION(InputHandler, EGameKey::Pinch, IE_Repeat, &ATDCPlayerController::OnPinchUpdate);
}

void ATDCPlayerController::BeginPlay()
{
	SpawnMainCharacter();

	PlayerCameraManager->SetViewTarget(GetPawn());
}


void ATDCPlayerController::SpawnMainCharacter()
{
	APlayerStart* currentPlayStart = nullptr;

	for (TObjectIterator<APlayerStart> It; It; ++It)
	{
		currentPlayStart = *It;

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		SpawnInfo.Owner = this;
		SpawnInfo.Instigator = NULL;
		SpawnInfo.bDeferConstruction = false;
					
		FVector spawnLocation(currentPlayStart->GetActorLocation());
		spawnLocation.Z = 110; // 10 units above the floor level. Feel free to make this configurable

		// Spawn the main character!
		MainCharacter = GetWorld()->SpawnActor<ATDCCharacter>(spawnLocation, 
			currentPlayStart->GetActorRotation(), SpawnInfo);

		MainCharacterController = GetWorld()->SpawnActor<ATDCAIController>(spawnLocation,
			currentPlayStart->GetActorRotation(), SpawnInfo);

		// Attach the main character to its AI Controller
		MainCharacterController->SetPawn(MainCharacter);
		MainCharacterController->Possess(MainCharacter);

		break; // don't create the character twice!
	}
}

void ATDCPlayerController::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
{
	if (!bGamePaused && PlayerInput && InputHandler && !bIgnoreInput)
	{
		InputHandler->UpdateDetection(DeltaTime);
	}

	Super::ProcessPlayerInput(DeltaTime, bGamePaused);
}

void ATDCPlayerController::UpdateRotation(float DeltaTime)
{
	FRotator ViewRotation(0, 0, 0);
	FRotator DeltaRot(0, 0, 0);

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
	}

	SetControlRotation(ViewRotation);
}

void ATDCPlayerController::MoveMainCharacterToLocation(FVector& location)
{
	if (MainCharacterController)
	{
		//location.Z = 0;
		MainCharacterController->MoveToLocation(location, -1.0f, true, true, true);
	}
}

void ATDCPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();

		bMoveToMouseCursor = false; // reset the flag immediately, there should be no reprocessing
	}

	if (GetSpectatorPawn()->GetFollowMainCharacter() && MainCharacter)
	{
		// TODO: do not create a new stack variable in the Tick method
		FVector newLocation(MainCharacter->GetActorLocation()); 

		//set the Z to avoid flickering. The value doesn't matter, because the camera has its own configuration.
		newLocation.Z = 800; 

		GetSpectatorPawn()->SetActorLocation(newLocation);
	}
	
}

void ATDCPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ATDCPlayerController::SetNewMoveDestination(FVector DestLocation)
{
	if (MainCharacter)
	{
		UNavigationSystemBase* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, MainCharacter->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (Distance > MinDistanceToMoveCharacter && GetCameraComponent())
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
			MoveMainCharacterToLocation(DestLocation);
		}
	}
}

void ATDCPlayerController::MoveToTouchLocationPressed(const ETouchIndex::Type FingerIndex, const FVector targetLocation)
{
	OnSetDestinationPressed();
/*	FVector2D TargetPositionOnScreen(targetLocation);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(TargetPositionOnScreen, CurrentClickTraceChannel, true, HitResult);

	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}*/
}

void ATDCPlayerController::MoveToTouchLocationReleased(const ETouchIndex::Type FingerIndex, const FVector targetLocation)
{
	OnSetDestinationReleased();
}

void ATDCPlayerController::OnSetDestinationPressed()
{
	bCandidateMoveToMouseCursor = true;
}

void ATDCPlayerController::OnSetDestinationReleased()
{
	if (bCandidateMoveToMouseCursor)
	{
		// set flag to keep updating destination until released
		bMoveToMouseCursor = true;
		bCandidateMoveToMouseCursor = false;
	}
}

void ATDCPlayerController::OnMouseScrollUp()
{
	ATDCSpectatorPawn* pawn = Cast<ATDCSpectatorPawn>(GetPawn());
	if (pawn)
	{
		pawn->OnMouseScrollUp();
	}
}

void ATDCPlayerController::OnMouseScrollDown()
{
	ATDCSpectatorPawn* pawn = Cast<ATDCSpectatorPawn>(GetPawn());
	if (pawn)
	{
		pawn->OnMouseScrollDown();
	}
}

void ATDCPlayerController::MoveForward(float Val)
{
	if (GetPawn() && MainCharacter)
	{
		GetSpectatorPawn()->MoveForward(Val);
		MainCharacterController->MoveToLocation(GetSpectatorPawn()->GetActorLocation());
	}
}


void ATDCPlayerController::MoveRight(float Val)
{
	if (GetPawn() && MainCharacter)
	{
		GetSpectatorPawn()->MoveRight(Val);
		MainCharacterController->MoveToLocation(GetSpectatorPawn()->GetActorLocation());
	}
}


/****************** Camera Input Handling **************************/

void ATDCPlayerController::OnTapPressed(const FVector2D& ScreenPosition, float DownTime)
{
	FVector WorldPosition(0.f);
	AActor* const HitActor = GetFriendlyTarget(ScreenPosition, WorldPosition);
	if (MainCharacter && HitActor && HitActor == MainCharacter)
	{
		GetSpectatorPawn()->SetFollowMainCharacter(true);
	}
}

void ATDCPlayerController::OnHoldPressed(const FVector2D& ScreenPosition, float DownTime)
{
	/*
	FVector WorldPosition(0.0f);
	AActor* const HitActor = GetFriendlyTarget(ScreenPosition, WorldPosition);

	SetSelectedActor(HitActor, WorldPosition);

	if (HitActor && HitActor->GetClass()->ImplementsInterface(UTDCInput::StaticClass()))
	{
		IStrategyInputInterface::Execute_OnInputHold(HitActor);
	}
	*/
}

void ATDCPlayerController::OnHoldReleased(const FVector2D& ScreenPosition, float DownTime)
{
	/*
	AActor* const Selected = SelectedActor.Get();
	if (Selected && Selected->GetClass()->ImplementsInterface(UTDCInput::StaticClass()))
	{
		IStrategyInputInterface::Execute_OnInputHoldReleased(Selected, DownTime);
	}
	*/
}

void ATDCPlayerController::OnSwipeStarted(const FVector2D& AnchorPosition, float DownTime)
{
	if (GetCameraComponent() != NULL)
	{
		GetCameraComponent()->OnSwipeStarted(AnchorPosition);
	}

	FVector WorldPosition(0.0f);
	AActor* const HitActor = GetFriendlyTarget(AnchorPosition, WorldPosition);

	SetSelectedActor(HitActor, WorldPosition);

	/** Get our position in 3d space */
	if (SelectedActor.IsValid())
	{
		SwipeAnchor3D = SelectedActor->GetActorLocation();
	}

	PrevSwipeScreenPosition = AnchorPosition;

	GetSpectatorPawn()->SetFollowMainCharacter(false);
}

void ATDCPlayerController::OnSwipeUpdate(const FVector2D& ScreenPosition, float DownTime)
{
	AActor* const Selected = SelectedActor.Get();
	if (Selected && Selected->GetClass()->ImplementsInterface(UTDCInput::StaticClass()))
	{
		ULocalPlayer* const MyPlayer = Cast<ULocalPlayer>(Player);
		const FPlane GroundPlane = FPlane(FVector(0, 0, SelectedActor->GetActorLocation().Z), FVector(0, 0, 1));

		FVector RayOrigin, RayDirection;
		FTDCCameraHelpers::DeprojectScreenToWorld(ScreenPosition, MyPlayer, RayOrigin, RayDirection);
		const FVector ScreenPosition3D = FTDCCameraHelpers::IntersectRayWithPlane(RayOrigin, RayDirection, GroundPlane);

		//ntk: IStrategyInputInterface::Execute_OnInputSwipeUpdate(Selected, ScreenPosition3D - SwipeAnchor3D);
	}
	else
	{
		if (GetCameraComponent() != NULL)
		{
			GetCameraComponent()->OnSwipeUpdate(ScreenPosition);

			bCandidateMoveToMouseCursor = false; // no character movement, only the camera changes position
		}
	}

	PrevSwipeScreenPosition = ScreenPosition;
}

void ATDCPlayerController::OnSwipeReleased(const FVector2D& ScreenPosition, float DownTime)
{
	AActor* const Selected = SelectedActor.Get();
	if (Selected && Selected->GetClass()->ImplementsInterface(UTDCInput::StaticClass()))
	{
		ULocalPlayer* const MyPlayer = Cast<ULocalPlayer>(this->Player);
		const FPlane GroundPlane = FPlane(FVector(0, 0, SelectedActor->GetActorLocation().Z), FVector(0, 0, 1));

		FVector RayOrigin, RayDirection;
		FTDCCameraHelpers::DeprojectScreenToWorld(ScreenPosition, MyPlayer, RayOrigin, RayDirection);
		const FVector ScreenPosition3D = FTDCCameraHelpers::IntersectRayWithPlane(RayOrigin, RayDirection, GroundPlane);

		//ntk: IStrategyInputInterface::Execute_OnInputSwipeReleased(Selected, ScreenPosition3D - SwipeAnchor3D, DownTime);
	}
	else
	{
		if (GetCameraComponent() != NULL)
		{
			GetCameraComponent()->OnSwipeReleased(ScreenPosition);
		}
	}
}

void ATDCPlayerController::OnSwipeTwoPointsStarted(const FVector2D& ScreenPosition1, const FVector2D& ScreenPosition2, float DownTime)
{
	PrevSwipeMidPoint = (ScreenPosition1 + ScreenPosition2) * 0.5f;
}

void ATDCPlayerController::OnSwipeTwoPointsUpdate(const FVector2D& ScreenPosition1, const FVector2D& ScreenPosition2, float DownTime)
{
	const FVector2D SwipeMidPoint = (ScreenPosition1 + ScreenPosition2) * 0.5f;
	const FVector MoveDir = FVector(SwipeMidPoint - PrevSwipeMidPoint, 0.0f).GetSafeNormal();
	const float SwipeSpeed = 10000.0f;

	const FRotationMatrix R(PlayerCameraManager->GetCameraRotation() + FRotator(0.0, 90.0, 0.0));
	const FVector WorldSpaceAccel = R.TransformVector(MoveDir) * SwipeSpeed;
	if (GetSpectatorPawn())
	{
		GetSpectatorPawn()->AddMovementInput(WorldSpaceAccel, 1.f);
	}

	PrevSwipeMidPoint = SwipeMidPoint;
}

void ATDCPlayerController::OnPinchStarted(const FVector2D& AnchorPosition1, const FVector2D& AnchorPosition2, float DownTime)
{
	// Pass the pinch through to the camera component.
	if (GetCameraComponent() != NULL)
	{
		GetCameraComponent()->OnPinchStarted(AnchorPosition1, AnchorPosition2, DownTime);

		// on pinch we can still follow the main character, just from a different zoom level, 
		// so leave the flag GetSpectatorPawn()->bFollowMainCharacter intact
	}
}

void ATDCPlayerController::OnPinchUpdate(const FVector2D& ScreenPosition1, const FVector2D& ScreenPosition2, float DownTime)
{
	// Pass the pinch through to the camera component.
	if (GetCameraComponent() != NULL)
	{
		GetCameraComponent()->OnPinchUpdate(InputHandler, ScreenPosition1, ScreenPosition2, DownTime);
	}
}

ATDCSpectatorPawn* ATDCPlayerController::GetSpectatorPawn() const
{
	APawn* pawn = GetPawn();
	return Cast<ATDCSpectatorPawn>(pawn);
}

UTDCCameraComponent* ATDCPlayerController::GetCameraComponent() const
{
	UTDCCameraComponent* CameraComponent = NULL;
	if (GetSpectatorPawn() != NULL)
	{
		CameraComponent = GetSpectatorPawn()->GetCameraComponent();
	}
	return CameraComponent;
}

void ATDCPlayerController::SetCameraTarget(const FVector& CameraTarget)
{
	if (GetCameraComponent() != NULL)
	{
		GetCameraComponent()->SetCameraTarget(CameraTarget);
	}
}

AActor* ATDCPlayerController::GetFriendlyTarget(const FVector2D& ScreenPoint, FVector& WorldPoint) const
{
	FHitResult Hit;
	if (GetHitResultAtScreenPosition(ScreenPoint, COLLISION_WEAPON, true, Hit))
	{
		WorldPoint = Hit.ImpactPoint;
		return Hit.GetActor();
	}

	return NULL;
}

void ATDCPlayerController::SetIgnoreInput(bool bIgnore)
{
	bIgnoreInput = bIgnore;
}

void ATDCPlayerController::SetSelectedActor(AActor* NewSelectedActor, const FVector& NewPosition)
{
	if (SelectedActor != NewSelectedActor)
	{
		SelectedActor = NewSelectedActor;
	}
}
