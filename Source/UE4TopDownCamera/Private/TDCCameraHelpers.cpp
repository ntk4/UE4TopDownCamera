// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "UE4TopDownCamera.h"
#include "TDCCameraHelpers.h"

bool FTDCCameraHelpers::DeprojectScreenToWorld(const FVector2D& ScreenPosition, ULocalPlayer* Player, FVector& RayOrigin, FVector& RayDirection)
{
	if (Player != NULL && Player->ViewportClient != NULL && Player->ViewportClient->Viewport != NULL && Player->PlayerController != NULL)
	{
		//get the projection data
		FSceneViewProjectionData ProjectionData;
		if (Player->GetProjectionData(Player->ViewportClient->Viewport, eSSP_FULL, /*out*/ ProjectionData))
		{
			const FMatrix InvViewMatrix = ProjectionData.ViewMatrix.InverseFast();
			const FMatrix InvProjectionMatrix = ProjectionData.ProjectionMatrix.InverseFast();

			FSceneView::DeprojectScreenToWorld(ScreenPosition, ProjectionData.GetConstrainedViewRect(), InvViewMatrix, InvProjectionMatrix, /*out*/ RayOrigin, /*out*/ RayDirection);

			return true;
		}
	}

	return false;
}

FVector FTDCCameraHelpers::IntersectRayWithPlane(const FVector& RayOrigin, const FVector& RayDirection, const FPlane& Plane)
{
	const FVector PlaneNormal = FVector(Plane.X, Plane.Y, Plane.Z);
	const FVector PlaneOrigin = PlaneNormal * Plane.W;

	const float Distance = FVector::DotProduct((PlaneOrigin - RayOrigin), PlaneNormal) / FVector::DotProduct(RayDirection, PlaneNormal);
	return RayOrigin + RayDirection * Distance;
}

TSharedPtr<TArray<uint8>> FTDCCameraHelpers::CreateAlphaMapFromTexture(UTexture2D* Texture)
{
	TSharedPtr<TArray<uint8>> ResultArray;

	// create temporary hitmask
	if (Texture && Texture->GetPixelFormat() == PF_B8G8R8A8 && Texture->GetNumMips() == 1)
	{
		const int32 HitMaskSize = Texture->GetSizeX() * Texture->GetSizeY();

		ResultArray = TSharedPtr<TArray<uint8>>(new TArray<uint8>());
		ResultArray->Init(255, HitMaskSize);
	}

	return ResultArray;
}

FCanvasUVTri FTDCCameraHelpers::CreateCanvasTri(FVector2D V0, FVector2D V1, FVector2D V2)
{
	FCanvasUVTri OutTri;
	OutTri.V0_Pos = V0;
	OutTri.V1_Pos = V1;
	OutTri.V2_Pos = V2;
	return OutTri;
}
