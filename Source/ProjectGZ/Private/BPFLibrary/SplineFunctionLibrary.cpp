// Fill out your copyright notice in the Description page of Project Settings.


#include "BPFLibrary/SplineFunctionLibrary.h"
#include "Components/SplineComponent.h"

void USplineFunctionLibrary::UpdateOverrideComponent(USplineComponent* spline)
{
	if (spline != nullptr) {
		spline->bModifiedByConstructionScript = true;	
		spline->bSplineHasBeenEdited = true;		
		spline->bInputSplinePointsToConstructionScript = true;
		spline->MarkPackageDirty();
		spline->MarkRenderStateDirty();
		spline->UpdateSpline();
	}
}
