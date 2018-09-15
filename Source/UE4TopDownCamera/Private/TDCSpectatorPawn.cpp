// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "UE4TopDownCamera.h"
#include "TDCPlayerController.h"
#include "TDCSpectatorPawn.h"


#define DEFAULT_ARM_LENGTH 800

ATDCSpectatorPawn::ATDCSpectatorPawn(const FObjectInitializer& OI)
	: Super(OI.SetDefaultSubobjectClass<USpectatorPawnMovement>(Super::MovementComponentName))
{
	GetCollisionComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	bAddDefaultMovementBindings = true;
	bFollowMainCharacter = true;

	CameraBoomComp = OI.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoomComp->SocketOffset = FVector(0, 0, 0); // how far the arm will be from the TargetArmLength on each axis
	CameraBoomComp->TargetOffset = FVector(0, 0, 0); // where character resides
	CameraBoomComp->SetRelativeRotation(FRotator(-85, 0, 0)); // rotation of the camera itself on Y so as to look at the character
	CameraBoomComp->TargetArmLength = DEFAULT_ARM_LENGTH; // distance behind the character that the camera is placed
	CameraBoomComp->bEnableCameraLag = true; //enable lag for more realistic camera
	CameraBoomComp->CameraLagSpeed = 4; // the lower the lagier, 10=default, 1=max lag, but 0=no lag
	CameraBoomComp->bUsePawnControlRotation = false;
	CameraBoomComp->bInheritYaw = false; // don't rotate the dungeon when the character rotates
	CameraBoomComp->SetupAttachment(GetRootComponent());

	CameraComponent = OI.CreateDefaultSubobject<UTDCCameraComponent>(this, TEXT("Camera"));

	//CameraComponent->SetRelativeRotation(FRotator(-85, 0, 0)); // rotation of the camera itself on Y so as to look at the character
	CameraComponent->SetupAttachment(CameraBoomComp);
}

void ATDCSpectatorPawn::MoveForward(float Val)
{
	Super::MoveForward(Val);

}
void ATDCSpectatorPawn::OnMouseScrollUp()
{
	CameraComponent->OnZoomOut();
}

void ATDCSpectatorPawn::OnMouseScrollDown()
{
	CameraComponent->OnZoomIn();
}

UTDCCameraComponent* ATDCSpectatorPawn::GetCameraComponent()
{
	check(CameraComponent != NULL);
	return CameraComponent;
}

