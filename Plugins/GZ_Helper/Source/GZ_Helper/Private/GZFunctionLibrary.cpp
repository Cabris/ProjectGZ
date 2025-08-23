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

	return nullptr;
}

bool UGZFunctionLibrary::GZHelper_GetViewPointFromActor(const AActor* Actor, FVector& OutLocation, FVector& OutDirection)
{
	if (!IsValid(Actor))return false;
	// 嘗試從 PlayerController 獲取視點
	if (const APawn* Pawn = Cast<APawn>(Actor))
	{
		if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
		{
			FRotator ViewRotation;
			PC->GetPlayerViewPoint(OutLocation, ViewRotation);
			OutDirection = ViewRotation.Vector();
			return true;
		}
	}

	// 後備方案：使用 Actor 的位置和前向量
	OutLocation = Actor->GetActorLocation();
	OutDirection = Actor->GetActorForwardVector();
	return true;
}
