// Fill out your copyright notice in the Description page of Project Settings.
#include "AbilitySystem/Ability/GZStartInteractionAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "Interfactions/GZInteractable.h"
#include "AbilitySystem/Ability/GZInteractAbility.h"
#include "Character/GZCharacterBase.h"

UGZStartInteractionAbility::UGZStartInteractionAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// 這個能力通常是「常駐被動」，可搭配授與時機（如角色初始化）給予
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	Hits.Reserve(20);
}

void UGZStartInteractionAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TraceTimerHandle,
			FTimerDelegate::CreateUObject(this, &ThisClass::TickTrace),
			TraceInterval,
			true);
	}
}

void UGZStartInteractionAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TraceTimerHandle);
	}

	// 收尾焦點
	if (LastActiveActor.IsValid())
	{
		EndFocus(LastActiveActor.Get());
	}
	ActiveActor = nullptr;
	LastActiveActor = nullptr;

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UGZStartInteractionAbility::TickTrace()
{
	FVector ViewLoc, ViewDir;
	if (!GetViewPoint(ViewLoc, ViewDir)) return;

	const FVector Start = ViewLoc;
	const FVector End = Start + ViewDir * TraceDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(GZInteractTrace), /*bTraceComplex*/ false, GetAvatarActorFromActorInfo());
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);

	Hits.Reset();
	const bool bHit = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, TraceChannel, Sphere, Params);

	AActor* Best = nullptr;

	if (bHit)
	{
		float BestScore = -FLT_MAX;
		for (const FHitResult& Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			IGZInteractable* Interactable = Cast<IGZInteractable>(HitActor);
			if (!Interactable || !Interactable->IsInteractable())
				continue;

			const float Score = ScoreCandidate(ViewLoc, ViewDir, Interactable->GetWorldPosition());
			if (Score > BestScore)
			{
				BestScore = Score;
				Best = HitActor;
			}
		}
	}

	// 焦點切換
	AActor* NewActive = Best;
	if (LastActiveActor.IsValid() && LastActiveActor.Get() != NewActive)
	{
		EndFocus(LastActiveActor.Get());
	}
	if (NewActive && (!LastActiveActor.IsValid() || LastActiveActor.Get() != NewActive))
	{
		BeginFocus(NewActive);
	}

	ActiveActor = NewActive;
	LastActiveActor = NewActive;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	DrawDebugSphere(GetWorld(), End, TraceRadius, 16, FColor::Green, false, TraceInterval);
#endif
}

void UGZStartInteractionAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AttemptInteraction_Internal();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, /*bReplicateEndAbility*/ true, /*bWasCancelled*/ false);
}

void UGZStartInteractionAbility::AttemptInteraction_Internal()
{
	AActor* Target = ActiveActor.Get();
	if (!Target) return;
	IGZInteractable* Interactable = Cast<IGZInteractable>(Target);
	if (!Interactable || !Interactable->IsInteractable())
		return;

	UGZAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	AActor* ASCOwnerActor = ASC->GetOwnerActor();
	AActor* ASCAvatarActor =ASC->GetAvatarActor();
	TSubclassOf<UGZInteractAbility> InteractAbilityClass = Interactable->GetInteractAbilityClass();
	FGameplayTag InteractionTag = InteractAbilityClass.GetDefaultObject()->InteractionTag;
	FGameplayEventData Evt;
	Evt.EventTag = InteractionTag;
	Evt.Instigator = ASCAvatarActor; // 誰發起
	Evt.Target = Target; // 互動對象
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(ASCOwnerActor, InteractionTag, Evt);
 
}

bool UGZStartInteractionAbility::GetViewPoint(FVector& OutLoc, FVector& OutDir) const
{
	if (const AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		if (const APawn* Pawn = Cast<APawn>(Avatar))
		{
			if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
			{
				FRotator ViewRot;
				PC->GetPlayerViewPoint(OutLoc, ViewRot);
				OutDir = ViewRot.Vector();
				return true;
			}
		}
		// 後備：用 Avatar 面向
		OutLoc = Avatar->GetActorLocation();
		OutDir = Avatar->GetActorForwardVector();
		return true;
	}
	return false;
}

float UGZStartInteractionAbility::ScoreCandidate(const FVector& ViewLoc, const FVector& ViewDir, const FVector& TargetLoc) const
{
	const FVector ToTarget = (TargetLoc - ViewLoc);
	const float Dist = ToTarget.Size();
	const FVector ToTargetDir = ToTarget.GetSafeNormal();

	const float Aim = (FVector::DotProduct(ViewDir, ToTargetDir) * 0.5f) + 0.5f;
	const float DistScore = 1.f / FMath::Max(1.f, Dist);

	return AimWeight * Aim + DistanceWeight * DistScore;
}

void UGZStartInteractionAbility::BeginFocus(AActor* NewActor)
{
	if (NewActor)
	{
		IGZInteractable::Execute_OnBeginFocus(NewActor);
	}
}

void UGZStartInteractionAbility::EndFocus(AActor* OldActor)
{
	if (OldActor)
	{
		IGZInteractable::Execute_OnEndFocus(OldActor);
	}
}
