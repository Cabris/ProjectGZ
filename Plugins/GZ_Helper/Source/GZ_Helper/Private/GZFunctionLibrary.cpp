// Fill out your copyright notice in the Description page of Project Settings.


#include "GZFunctionLibrary.h"
#include "Components/SplineComponent.h"


void UGZFunctionLibrary::GZHelper_NotifySplineUpdate(USplineComponent* Spline)
{
	if (Spline != nullptr)
	{
		Spline->bModifiedByConstructionScript = true;
		Spline->bSplineHasBeenEdited = true;
		Spline->bInputSplinePointsToConstructionScript = true;
		Spline->MarkPackageDirty();
		Spline->MarkRenderStateDirty();
		Spline->UpdateSpline();
	}
}

AActor* UGZFunctionLibrary::GZHelper_GetActorFromSotfPath(const FSoftObjectPath& SoftObjectPath)
{
	TSoftObjectPtr<AActor> ref = TSoftObjectPtr<AActor>(SoftObjectPath);
	UE_LOG(LogTemp, Warning, TEXT("UPCGFunctionLibrary:ref.IsValid %d" ), ref.IsValid());
	if (ref.IsValid())
	{
		auto actor = ref.LoadSynchronous();
		if (!actor)
		{
			UE_LOG(LogTemp, Warning, TEXT("UPCGFunctionLibrary:ref.LoadSynchronous Fail"));
		}
		return actor;
	}

	return  nullptr;
}
