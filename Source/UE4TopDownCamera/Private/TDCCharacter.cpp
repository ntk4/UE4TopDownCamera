// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UE4TopDownCamera.h"
#include "TDCCharacter.h"

// Sets default values
ATDCCharacter::ATDCCharacter(const class FObjectInitializer& OI)
	: Super(OI.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFinder(CHARACTER_MESH);
	if (MeshFinder.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshFinder.Object);
	}
	
	// Warning: The animation is often not resolved on the android deployment. To be fixed
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimFinder(CHARACTER_ANIMATION);
	if (AnimFinder.Object)
	{
		GetMesh()->SetAnimInstanceClass(AnimFinder.Object->GeneratedClass);
	}
	
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0)); // rotate the mesh to match the Arrow component
	GetMesh()->SetRelativeLocation(FVector(0, 0, -80)); // align the mesh inside the Capsule component
	
	// capsule values are based on the demo mesh currently used. Adjust these when the character is finalized
	GetCapsuleComponent()->SetCapsuleRadius(30.0f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(85.0f);


	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	// Adjust jump to make it less floaty
	//MoveComp->GravityScale = 1.5f;
	//MoveComp->JumpZVelocity = 620;
	MoveComp->bAlwaysCheckFloor = true;
	MoveComp->bConstrainToPlane = true;
	MoveComp->bCanWalkOffLedgesWhenCrouching = false;
	MoveComp->MaxWalkSpeedCrouched = 200;
	MoveComp->bCanWalkOffLedges = false;


	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = true;
	bUseControllerRotationYaw = true;

	/* Ignore this channel or it will absorb the trace impacts instead of the skeletal mesh */
	//ntk: GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	// Disable crouching, flying
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = false;
	MoveComp->GetNavAgentPropertiesRef().bCanFly = false;

}


// Called every frame
void ATDCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATDCCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


// Called to bind functionality to input
void ATDCCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// Movement
	InputComponent->BindAxis("MoveForward", this, &ATDCCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATDCCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}


void ATDCCharacter::MoveForward(float Val)
{
	if (Controller && Val != 0.f)
	{
		// Limit pitch when walking or falling
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

		AddMovementInput(Direction, Val);
	}
}


void ATDCCharacter::MoveRight(float Val)
{
	if (Val != 0.f)
	{
		const FRotator Rotation = GetActorRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}
