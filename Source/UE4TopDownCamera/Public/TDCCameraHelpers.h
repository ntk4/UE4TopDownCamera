
#pragma once

#include "UE4TopDownCamera.h"


/** when you modify this, please note that this information can be saved with instances
* also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list **/
#define COLLISION_WEAPON		ECC_GameTraceChannel1
#define COLLISION_PROJECTILE	ECC_GameTraceChannel2
#define COLLISION_PANCAMERA		ECC_GameTraceChannel3

class FTDCCameraHelpers
{
public:
	/** convert point in screen space to ray in world space */
	static bool DeprojectScreenToWorld(const FVector2D& ScreenPosition, class ULocalPlayer* Player, FVector& RayOrigin, FVector& RayDirection);

	/** find intersection of ray in world space with ground plane */
	static FVector IntersectRayWithPlane(const FVector& RayOrigin, const FVector& RayDirection, const FPlane& Plane);

	/** create alpha map from UTexture2D for hit-tests in Slate */
	static TSharedPtr<TArray<uint8>> CreateAlphaMapFromTexture(UTexture2D* Texture);

	/** creates FCanvasUVTri without UV from 3x FVector2D */
	static FCanvasUVTri CreateCanvasTri(FVector2D V0, FVector2D V1, FVector2D V2);
};
