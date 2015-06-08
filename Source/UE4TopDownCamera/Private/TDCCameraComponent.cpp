// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UE4TopDownCamera.h"
#include "TDCInput.h"
#include "TDCCameraHelpers.h"
#include "TDCSpectatorPawnMovement.h"
#include "TDCCameraComponent.h"

UTDCCameraComponent::UTDCCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// the default Zoom is hardcoded...because at the time this constructor is called we don't have the values from the DefaultGame.ini yet
	ZoomAlpha = 0.4f; 
	StartSwipeCoords.Set(0.0f, 0.0f, 0.0f);
}

void UTDCCameraComponent::OnZoomIn()
{
	SetZoomLevel(ZoomAlpha - 0.1f);
}

void UTDCCameraComponent::OnZoomOut()
{
	SetZoomLevel(ZoomAlpha + 0.1f);
}

void UTDCCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& OutResult)
{	
	APlayerController* Controller = GetPlayerController();
	if( Controller ) 
	{
		OutResult.FOV = 30.f;
		const float CurrentOffset = MinCameraOffset + ZoomAlpha * (MaxCameraOffset - MinCameraOffset);
		FVector Pos2 = Controller->GetFocalLocation();
		OutResult.Location = Controller->GetFocalLocation() - FixedCameraAngle.Vector() * CurrentOffset;
		OutResult.Rotation = FixedCameraAngle;
	}
}

void UTDCCameraComponent::UpdateCameraMovement( const APlayerController* InPlayerController )
{
	// No mouse support on mobile
#if PLATFORM_DESKTOP
	ULocalPlayer* const LocalPlayer =  Cast<ULocalPlayer>(InPlayerController->Player);
	if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->Viewport )
	{
		FVector2D MousePosition;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePosition) == false)
		{
			return;
		}

		FViewport* Viewport = LocalPlayer->ViewportClient->Viewport;
		const float ScrollSpeed = 60.0f;
		const FIntPoint ViewportSize = Viewport->GetSizeXY();
		const uint32 ViewLeft = FMath::TruncToInt(LocalPlayer->Origin.X * ViewportSize.X);
		const uint32 ViewRight = ViewLeft + FMath::TruncToInt(LocalPlayer->Size.X * ViewportSize.X);
		const uint32 ViewTop = FMath::TruncToInt(LocalPlayer->Origin.Y * ViewportSize.Y);
		const uint32 ViewBottom = ViewTop + FMath::TruncToInt(LocalPlayer->Size.Y * ViewportSize.Y);
		
		const float MaxSpeed = CameraSpeed * FMath::Clamp(ZoomAlpha, MinZoomLevel, MaxZoomLevel);

		bool bNoScrollZone = false;
		FVector MouseCoords( MousePosition, 0.0f );
		for (int iZone = 0; iZone < NoScrollZones.Num() ; iZone++)
		{
			FBox EachZone = NoScrollZones[ iZone ];
			if( EachZone.IsInsideXY( MouseCoords ) == true )
			{
				bNoScrollZone = true;
			}
		}

		const uint32 MouseX = MousePosition.X;
		const uint32 MouseY = MousePosition.Y;
		float SpectatorCameraSpeed = MaxSpeed;
		ASpectatorPawn* SpectatorPawn = NULL;
		if( GetPlayerController() != NULL )
		{
			SpectatorPawn = GetPlayerController()->GetSpectatorPawn();
			if( SpectatorPawn->GetMovementComponent() != NULL )
			{
				SpectatorCameraSpeed = GetDefault<UTDCSpectatorPawnMovement>(SpectatorPawn->GetMovementComponent()->GetClass())->MaxSpeed;
			}
		}
		if (!bNoScrollZone)
		{
			if (MouseX >= ViewLeft && MouseX <= (ViewLeft + CameraActiveBorder))
			{
				const float delta = 1.0f - float(MouseX - ViewLeft) / CameraActiveBorder;
				SpectatorCameraSpeed = delta * MaxSpeed;
				MoveRight(-ScrollSpeed * delta);
			}
			else if (MouseX >= (ViewRight - CameraActiveBorder) && MouseX <= ViewRight)
			{
				const float delta = float(MouseX - ViewRight + CameraActiveBorder) / CameraActiveBorder;
				SpectatorCameraSpeed = delta * MaxSpeed;
				MoveRight(ScrollSpeed * delta);
			}

			if (MouseY >= ViewTop && MouseY <= (ViewTop + CameraActiveBorder))
			{
				const float delta = 1.0f - float(MouseY - ViewTop) / CameraActiveBorder;
				SpectatorCameraSpeed = delta * MaxSpeed;
				MoveForward(ScrollSpeed * delta);
			}
			else if (MouseY >= (ViewBottom - CameraActiveBorder) && MouseY <= ViewBottom)
			{
				const float delta = float(MouseY - (ViewBottom - CameraActiveBorder)) / CameraActiveBorder;
				SpectatorCameraSpeed = delta * MaxSpeed;
				MoveForward(-ScrollSpeed * delta);
			}

			if( SpectatorPawn != NULL )
			{
				UFloatingPawnMovement* PawnMovementComponent = Cast<UFloatingPawnMovement>(SpectatorPawn->GetMovementComponent());
				if (PawnMovementComponent)
				{
					PawnMovementComponent->MaxSpeed = SpectatorCameraSpeed;
				}
			}
		}
	}
#endif
	NoScrollZones.Empty();
}

void UTDCCameraComponent::MoveXYZ(EAxis::Type Axis, float Val)
{
	APawn* OwnerPawn = GetOwnerPawn();
	if (OwnerPawn != NULL)
	{
		APlayerController* Controller = GetPlayerController();
		if ((Val != 0.f) && (Controller != NULL))
		{
			const FRotationMatrix R(Controller->PlayerCameraManager->GetCameraRotation());
			const FVector WorldSpaceAccel = R.GetScaledAxis(Axis) * 100.0f;

			// transform to world space and add it
			OwnerPawn->AddMovementInput(WorldSpaceAccel, Val);
		}
	}

}

void UTDCCameraComponent::AddNoScrollZone( FBox InCoords )
{
	NoScrollZones.AddUnique( InCoords );
}

void UTDCCameraComponent::ClampCameraLocation( const APlayerController* InPlayerController, FVector& OutCameraLocation )
{	
	if (bShouldClampCamera)
	{
		UpdateCameraBounds(InPlayerController);
		if (CameraMovementBounds.GetSize() != FVector::ZeroVector)
		{
			OutCameraLocation = CameraMovementBounds.GetClosestPointTo(OutCameraLocation);
		}
	}
}

void UTDCCameraComponent::UpdateCameraBounds( const APlayerController* InPlayerController )
{
	// this used to do some stuff in the StrategyGame sample for the minimap
}

APlayerController* UTDCCameraComponent::GetPlayerController()
{
	APlayerController* Controller = NULL;
	APawn* Owner = GetOwnerPawn();
	if( Owner != NULL )
	{
		Controller = Cast<APlayerController>(Owner->GetController());
	}
	return Controller;
}

APawn* UTDCCameraComponent::GetOwnerPawn()
{
	return Cast<APawn>(GetOwner());
}

void UTDCCameraComponent::OnPinchStarted(const FVector2D& AnchorPosition1, const FVector2D& AnchorPosition2, float DownTime)
{
	InitialPinchAlpha = ZoomAlpha;
}

static TAutoConsoleVariable<float> CVarPinchScale(TEXT("PinchScale"), 0.002f, TEXT("How fast strategy pinch is."));

void UTDCCameraComponent::OnPinchUpdate(UTDCInput* InputHandler, const FVector2D& ScreenPosition1, const FVector2D& ScreenPosition2, float DownTime)
{
	const float AnchorDistance = (InputHandler->GetTouchAnchor(0) - InputHandler->GetTouchAnchor(1)).Size();
	const float CurrentDistance = (ScreenPosition1 - ScreenPosition2).Size();
	const float PinchDelta = AnchorDistance - CurrentDistance;
	const float PinchScale = CVarPinchScale.GetValueOnGameThread();
	
	SetZoomLevel(InitialPinchAlpha + PinchDelta * PinchScale);
}

void UTDCCameraComponent::SetCameraTarget(const FVector& CameraTarget)
{
	ASpectatorPawn* SpectatorPawn = GetPlayerController()->GetSpectatorPawn();
	if( SpectatorPawn != NULL )
	{
		SpectatorPawn->SetActorLocation(CameraTarget, false);
	}	
}

void UTDCCameraComponent::SetZoomLevel(float NewLevel)
{
	ZoomAlpha = FMath::Clamp(NewLevel, MinZoomLevel, MaxZoomLevel);
}

bool UTDCCameraComponent::OnSwipeStarted(const FVector2D& SwipePosition)
{
	bool bResult = false;
	// Ensure we are NOT trying to start a drag/scroll over a no scroll zone (EG mini map)
	if (AreCoordsInNoScrollZone(SwipePosition) == false)
	{
		APlayerController* Controller = GetPlayerController();
		if (Controller)
		{
			// Get intersection point with the plan used to move around
			FHitResult Hit;
			if (Controller->GetHitResultAtScreenPosition(SwipePosition, COLLISION_PANCAMERA, true, Hit))
			{
				StartSwipeCoords = Hit.ImpactPoint;
				bResult = true;
			}
		}
	}
	else
	{
		EndSwipeNow();
	}
	return bResult;
}

bool UTDCCameraComponent::OnSwipeUpdate(const FVector2D& SwipePosition)
{
	bool bResult = false;
	APlayerController* Controller = GetPlayerController();
	if ((Controller != NULL) && (StartSwipeCoords.IsNearlyZero() == false ) )
	{
		FHitResult Hit;
		if (Controller->GetHitResultAtScreenPosition(SwipePosition, COLLISION_PANCAMERA, true, Hit))
		{
			FVector NewSwipeCoords = Hit.ImpactPoint;
			FVector Delta = StartSwipeCoords - NewSwipeCoords;
			// Flatten Z axis - we are not interested in that.
			Delta.Z = 0.0f;
			if (Delta.IsNearlyZero() == false)
			{
				ASpectatorPawn* SpectatorPawn = Cast<ASpectatorPawn>(GetPlayerController()->GetPawn());
				if (SpectatorPawn != NULL)
				{
					FVector CurrentCamera = SpectatorPawn->GetActorLocation();
					CurrentCamera += Delta;
					SpectatorPawn->SetActorLocation(CurrentCamera);
					SetCameraTarget(CurrentCamera);
					bResult = true;
				}
			}
		}
	}
	return bResult;
}

bool UTDCCameraComponent::OnSwipeReleased(const FVector2D& SwipePosition)
{
	bool bResult = false;
	if (StartSwipeCoords.IsNearlyZero() == false)
	{
		APlayerController* Controller = GetPlayerController();
		if (Controller)
		{
			FHitResult Hit;
			if (Controller->GetHitResultAtScreenPosition(SwipePosition, COLLISION_PANCAMERA, true, Hit))
			{
				FVector EndSwipeCoords = Hit.ImpactPoint;
				bResult = true;
			}
		}
		EndSwipeNow();
	}

	return false;
}

void UTDCCameraComponent::EndSwipeNow()
{
	StartSwipeCoords.Set(0.0f, 0.0f, 0.0f);
}

bool UTDCCameraComponent::AreCoordsInNoScrollZone(const FVector2D& SwipePosition)
{
	bool bResult = false;
	FVector MouseCoords(SwipePosition, 0.0f);
	for (int iZone = 0; iZone < NoScrollZones.Num(); iZone++)
	{
		FBox EachZone = NoScrollZones[iZone];
		if (EachZone.IsInsideXY(MouseCoords) == true)
		{
			bResult = true;
		}
	}
	return bResult;
}
