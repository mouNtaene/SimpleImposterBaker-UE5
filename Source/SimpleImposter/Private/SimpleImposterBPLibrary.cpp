// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleImposterBPLibrary.h"
#include "SimpleImposter.h"
#include "Engine/TextureRenderTarget2D.h"
#include "OpenCVHelper.h"
#include "UDynamicMesh.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "GeometryScript/GeometryScriptTypes.h"
#include "DynamicMesh/DynamicMeshAttributeSet.h"


#if WITH_OPENCV
// IWYU pragma: begin_keep
#include "PreOpenCVHeaders.h"
#include "opencv2/imgproc.hpp"
#include "PostOpenCVHeaders.h"

// IWYU pragma: end_keep
#endif	// WITH_OPENCV



USimpleImposterBPLibrary::USimpleImposterBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void USimpleImposterBPLibrary::UpdateRTResource(UTextureRenderTarget2D* RenderTarget, bool bClearRenderTarget)
{
	if (RenderTarget)
	{
		RenderTarget->UpdateResourceImmediate(bClearRenderTarget); //force update, for some reason only this way rt mip maps are generated
	}
}

bool USimpleImposterBPLibrary::FindContour(const UTextureRenderTarget2D* RenderTarget, TArray<FContour>& ContourPoints, const float Epsilon)
{
	
	if (RenderTarget)
	{
		
		if (RenderTarget->RenderTargetFormat == ETextureRenderTargetFormat::RTF_RGBA8)
		{
			FRenderTarget* RenderTargetLocal = ((UTextureRenderTarget2D*)RenderTarget)->GameThread_GetRenderTargetResource();
			if (RenderTargetLocal)
			{
				// Copy the pixels from the RenderTarget
				TArray<FColor> Pixels;
				if (RenderTargetLocal->ReadPixels(Pixels))
				{
					// Check for invalid dimensions
					const FIntPoint Size = RenderTargetLocal->GetSizeXY();
					// Create OpenCV Mat with those pixels and convert to grayscale for image processing
					cv::Mat CvFrameColor(cv::Size(Size.X, Size.Y), CV_8UC4, Pixels.GetData());
					cv::Mat CvFrameGray, Thresholded;
					cv::cvtColor(CvFrameColor, CvFrameGray, cv::COLOR_RGBA2GRAY);
					//cv::threshold(CvFrameGray, Thresholded, 50, 255, cv::THRESH_BINARY);
					std::vector<std::vector<cv::Point>> LocalContours;
					cv::findContours(CvFrameGray, LocalContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
					size_t ContoursSize = LocalContours.size();
					std::vector<std::vector<cv::Point>> LocalApprox{ ContoursSize };

					if (ContoursSize > 0)
					{
						for (size_t i = 0; i < ContoursSize; i++)
						{
							float LocalEps = cv::arcLength(LocalContours[i], true) * Epsilon; //calculate length 
							cv::approxPolyDP(LocalContours[i], LocalApprox[i],LocalEps,true); // approximate contours, they are most likely dense

						}
						std::vector<std::vector<cv::Point>> LocalHulls{ LocalApprox.size() };
						for (size_t i = 0; i< LocalApprox.size(); i++)
						{
							cv::convexHull(LocalApprox[i], LocalHulls[i]); //final approximation 
						}
						int HullsNum = LocalHulls.size();
						if (HullsNum > 0)
						{
							for (const auto& Hull : LocalHulls)
							{
								TArray<FVector2D> LocalPoints;
								for (const auto& InnerHull : Hull)
								{
									const FVector2D Point{ (float)InnerHull.x / (float)Size.X, float(InnerHull.y) / (float)Size.Y }; //to 0-1 range 
									LocalPoints.Add(Point);
								}
								const FContour LocalContour{ LocalPoints };
								ContourPoints.Add(LocalContour);
							}
							return true;
						}
					}
					
				}
			}
		}
		
	}
	return false;
 }


UDynamicMesh* USimpleImposterBPLibrary::SetMeshUVs(UDynamicMesh* TargetMesh,UPARAM(DisplayName = "UV Channel") int UVSetIndex,const TArray<FVector2D>& UV)
{
	if (TargetMesh)
	{
		TargetMesh->EditMesh([&](FDynamicMesh3& EditMesh)
		{
			if (EditMesh.HasAttributes() && UVSetIndex < EditMesh.Attributes()->NumUVLayers())
			{

				UE::Geometry::FDynamicMeshUVOverlay* UVOverlay = EditMesh.Attributes()->GetUVLayer(UVSetIndex); 
				if (UVOverlay)
				{
					if (EditMesh.MaxVertexID() == UV.Num()) {

						for (auto TId : EditMesh.TriangleIndicesItr())  //for each triangle..
						{ 

							UE::Geometry::FIndex3i TriElem = EditMesh.GetTriangle(TId);

							int32 Elem0 = UVOverlay->AppendElement((FVector2f)UV[TriElem.A]);
							int32 Elem1 = UVOverlay->AppendElement((FVector2f)UV[TriElem.B]);
							int32 Elem2 = UVOverlay->AppendElement((FVector2f)UV[TriElem.C]);
							const UE::Geometry::FIndex3i UvElem{ Elem0,Elem1,Elem2 };

							UVOverlay->SetTriangle(TId, UvElem, true);
						}
					}
					else {
						UE_LOG(LogTemp, Warning, TEXT("Vertex and UV count is differ!"));
					}
				}

			}
			
		}, EDynamicMeshChangeType::GeneralEdit, EDynamicMeshAttributeChangeFlags::Unknown, false);
	}
	return TargetMesh;
}