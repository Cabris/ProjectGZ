#include "AbilitySystem/GZInteractionDetector.h"
#include "Interfactions/GZInteractable.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "GZFunctionLibrary.h"

UGZInteractionDetector::UGZInteractionDetector()
{
	HitResults.Reserve(20);
}

void UGZInteractionDetector::Initialize(UWorld* World, AActor* InOwnerActor, const FGZInteractionDetectorConfig& InConfig)
{
	if (!World || !InOwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UGZInteractionDetector::Initialize] Invalid World or OwnerActor"));
		return;
	}

	// 清理舊的狀態
	StopDetection();
	
	CachedWorld = World;
	OwnerActor = InOwnerActor;
	Config = InConfig;
	CurrentFocusActor = nullptr;

	UE_LOG(LogTemp, Log, TEXT("[UGZInteractionDetector::Initialize] Initialized for Actor: %s"), 
		*InOwnerActor->GetName());
}

void UGZInteractionDetector::StartDetection()
{
	if (!CachedWorld.IsValid() || !OwnerActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UGZInteractionDetector::StartDetection] Invalid World or OwnerActor"));
		return;
	}

	// 如果已經在檢測中，先停止
	StopDetection();

	// 啟動定時器
	FTimerManager& TimerManager = CachedWorld->GetTimerManager();
	TimerManager.SetTimer(TraceTimerHandle, this, &UGZInteractionDetector::PerformDetection, 
		Config.TraceInterval, true);

	UE_LOG(LogTemp, Log, TEXT("[UGZInteractionDetector::StartDetection] Detection started with interval: %f"), 
		Config.TraceInterval);
}

void UGZInteractionDetector::StopDetection()
{
	if (CachedWorld.IsValid() && TraceTimerHandle.IsValid())
	{
		FTimerManager& TimerManager = CachedWorld->GetTimerManager();
		TimerManager.ClearTimer(TraceTimerHandle);
	}

	// 清理當前焦點
	if (CurrentFocusActor.IsValid())
	{
		HandleFocusChange(nullptr);
	}

	TraceTimerHandle.Invalidate();
	
	UE_LOG(LogTemp, Log, TEXT("[UGZInteractionDetector::StopDetection] Detection stopped"));
}

void UGZInteractionDetector::ForceUpdate()
{
	if (CachedWorld.IsValid() && OwnerActor.IsValid())
	{
		PerformDetection();
	}
}

void UGZInteractionDetector::UpdateConfig(const FGZInteractionDetectorConfig& NewConfig)
{
	const bool bWasActive = IsDetectionActive();
	
	// 如果檢測間隔改變，需要重啟定時器
	if (bWasActive && !FMath::IsNearlyEqual(Config.TraceInterval, NewConfig.TraceInterval))
	{
		StopDetection();
		Config = NewConfig;
		StartDetection();
	}
	else
	{
		Config = NewConfig;
	}

	UE_LOG(LogTemp, Log, TEXT("[UGZInteractionDetector::UpdateConfig] Config updated"));
}

void UGZInteractionDetector::PerformDetection()
{
	if (!CachedWorld.IsValid() || !OwnerActor.IsValid())
	{
		StopDetection();
		return;
	}

	FVector ViewLocation, ViewDirection;
	if (!UGZFunctionLibrary::GZHelper_GetViewPointFromActor(OwnerActor.Get(),ViewLocation,ViewDirection))
	{
		return;
	}

	const FVector TraceStart = ViewLocation;
	const FVector TraceEnd = TraceStart + ViewDirection * Config.TraceDistance;
	
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(GZInteractionDetector), 
		/*bTraceComplex*/ false, OwnerActor.Get());
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(Config.TraceRadius);

	HitResults.Reset();
	CachedWorld->SweepMultiByChannel(HitResults, TraceStart, TraceEnd, FQuat::Identity, 
		Config.TraceChannel, SphereShape, QueryParams);

	// 尋找最佳候選目標
	AActor* BestCandidate = nullptr;
	float BestScore = -FLT_MAX;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || !IsActorInteractable(HitActor))
		{
			continue;
		}

		const IGZInteractable* Interactable = Cast<IGZInteractable>(HitActor);
		if (!Interactable)
		{
			continue;
		}

		const float Score = ScoreCandidate(ViewLocation, ViewDirection, Interactable->GetWorldPosition());
		if (Score > BestScore)
		{
			BestScore = Score;
			BestCandidate = HitActor;
		}
	}

	// 處理焦點變更
	if (CurrentFocusActor.Get() != BestCandidate)
	{
		HandleFocusChange(BestCandidate);
	}

	// Debug 繪製
	if (Config.bEnableDebugDraw)
	{
		DrawDebugSphere(CachedWorld.Get(), TraceStart, Config.TraceRadius, 16, FColor::Red, false, Config.TraceInterval);
		DrawDebugSphere(CachedWorld.Get(), TraceEnd, Config.TraceRadius, 16, FColor::Green, false, Config.TraceInterval);
		
		if (BestCandidate)
		{
			DrawDebugLine(CachedWorld.Get(), TraceStart, BestCandidate->GetActorLocation(), 
				FColor::Yellow, false, Config.TraceInterval, 0, 2.0f);
		}
	}
}


float UGZInteractionDetector::ScoreCandidate(const FVector& ViewLocation, const FVector& ViewDirection, const FVector& TargetLocation) const
{
	const FVector ToTarget = (TargetLocation - ViewLocation);
	const float Distance = ToTarget.Size();
	
	if (Distance <= KINDA_SMALL_NUMBER)
	{
		return 0.0f;
	}

	const FVector ToTargetDirection = ToTarget.GetSafeNormal();
	
	// 計算準心對準度 (0-1)
	const float DotProduct = FVector::DotProduct(ViewDirection, ToTargetDirection);
	const float AimScore = (DotProduct * 0.5f) + 0.5f;
	
	// 計算距離分數 (越近越高)
	const float DistanceScore = 1.0f / FMath::Max(1.0f, Distance);
	
	return Config.AimWeight * AimScore + Config.DistanceWeight * DistanceScore;
}

bool UGZInteractionDetector::IsActorInteractable(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	const IGZInteractable* Interactable = Cast<IGZInteractable>(Actor);
	return Interactable && Interactable->IsInteractable();
}

void UGZInteractionDetector::HandleFocusChange(AActor* NewFocus)
{
	AActor* OldFocus = CurrentFocusActor.Get();
	
	if (OldFocus == NewFocus)
	{
		return;
	}

	// 結束舊焦點
	if (OldFocus && IsValid(OldFocus))
	{
		IGZInteractable::Execute_OnEndFocus(OldFocus);
		
		UE_LOG(LogTemp, Log, TEXT("[UGZInteractionDetector] OnEndFocus: %s"), 
			*OldFocus->GetActorNameOrLabel());
	}

	// 開始新焦點
	if (NewFocus && IsValid(NewFocus))
	{
		IGZInteractable::Execute_OnBeginFocus(NewFocus);
		
		UE_LOG(LogTemp, Log, TEXT("[UGZInteractionDetector] OnBeginFocus: %s"), 
			*NewFocus->GetActorNameOrLabel());
	}

	CurrentFocusActor = NewFocus;
	
	// 觸發委託
	OnFocusChanged.Broadcast(NewFocus, OldFocus);
}

void UGZInteractionDetector::Cleanup()
{
	StopDetection();
	CachedWorld = nullptr;
	OwnerActor = nullptr;
	CurrentFocusActor = nullptr;
	OnFocusChanged.Clear();
}

void UGZInteractionDetector::BeginDestroy()
{
	Cleanup();
	Super::BeginDestroy();
}