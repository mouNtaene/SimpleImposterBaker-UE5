// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimpleImposterBPLibrary.generated.h"


class UTextureRenderTarget2D;
class UDynamicMesh;

USTRUCT(BlueprintType)
struct FContour
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SimpleImposter")
		TArray<FVector2D> Points;
	FContour() {}; //default constructor
	FContour(const TArray<FVector2D>& x) : Points(x) {};

};


UCLASS(BlueprintType)
class USimpleImposterBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GenerateRTMipMaps", Keywords = "MipMaps"), Category = "RenderTarget")
		static void UpdateRTResource(UTextureRenderTarget2D* RenderTarget, bool bClearRenderTarget);

	UFUNCTION(BlueprintCallable, meta = (DisplaynName = "FindContour"), Category = "ImageProcessing")
		static bool FindContour(const UTextureRenderTarget2D* RenderTarget, TArray<FContour>& ContourPoints,const float Epsilon = 0.02f);


	UFUNCTION(BlueprintCallable, Category = "GeometryScript|UVs", meta = (ScriptMethod))
		static UPARAM(DisplayName = "Target Mesh") UDynamicMesh* SetMeshUVs(
			UDynamicMesh* TargetMesh,
			UPARAM(DisplayName = "UV Channel") int UVSetIndex,
			const TArray<FVector2D>& UV);
	
};
