// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UE4TopDownCamera.h"
#include "TDCSpectatorPawn.h"
#include "TDCSpectatorPawnMovement.h"

UTDCSpectatorPawnMovement::UTDCSpectatorPawnMovement(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), 
	bInitialLocationSet(false)
{
	MaxSpeed = 16000.f;
	Acceleration = 5000.f;
	Deceleration = 4000.f;

}

void UTDCSpectatorPawnMovement::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(PawnOwner->GetController());
	if (PlayerController && PlayerController->IsLocalController())
	{
		if (!bInitialLocationSet)
		{
			PawnOwner->SetActorRotation(PlayerController->GetControlRotation());
			PawnOwner->SetActorLocation(PlayerController->GetSpawnLocation());
			bInitialLocationSet = true;
		}

		FVector MyLocation = UpdatedComponent->GetComponentLocation();
		ATDCSpectatorPawn* SpectatorPawn = Cast<ATDCSpectatorPawn>(PlayerController->GetSpectatorPawn());
		if ((SpectatorPawn != NULL) && (SpectatorPawn->GetCameraComponent() != NULL))
		{
			SpectatorPawn->GetCameraComponent()->ClampCameraLocation(PlayerController, MyLocation);
		}
		UpdatedComponent->SetWorldLocation(MyLocation, false);
	}
}