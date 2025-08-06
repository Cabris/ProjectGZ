#include "Actor/GZEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "Interfactions/GZAbilitySystemInterface.h"


AGZEffectActor::AGZEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGZEffectActor::ApplyEffectToTarget(AActor* TargetActor, FEffectParams& InEffectParams, float Level)
{
	check(TargetActor);
	IGZAbilitySystemInterface* ASI = Cast<IGZAbilitySystemInterface>(TargetActor);
	if (!ASI) return;
	UGZAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();
	if (!TargetASC)return;

	TSubclassOf<UGameplayEffect> GameplayEffectClass = InEffectParams.EffectClass;
	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, Level, EffectContextHandle);
	FGameplayEffectSpec* EffectSpec = EffectSpecHandle.Data.Get();
	FActiveGameplayEffectHandle EffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec);

	bool IsInfiniteEffect = EffectSpec->Def->DurationPolicy == EGameplayEffectDurationType::Infinite;
	bool ShouldRemoveEffectOnEndOverlap = InEffectParams.RemoveCondition == EEffectCondition::OnEndOverlap;
	if (IsInfiniteEffect && ShouldRemoveEffectOnEndOverlap)
	{
		//Infinite Effect won't self expire, so EffectHandle needs to be store for later remove.
		ActiveEffectHandles.Add(EffectHandle, TargetASC);
	}
}

void AGZEffectActor::OnOverlap(AActor* TargetActor)
{
	for (FEffectParams& EffectParam : Effects)
	{
		if (!IsValid(EffectParam.EffectClass)) continue;
		if (EffectParam.ApplyCondition == EEffectCondition::OnOverlap)
			ApplyEffectToTarget(TargetActor, EffectParam, ActorLevel);
	}
}

void AGZEffectActor::OnEndOverlap(AActor* TargetActor)
{
	for (FEffectParams& EffectParam : Effects)
	{
		if (!IsValid(EffectParam.EffectClass)) continue;
		if (EffectParam.ApplyCondition == EEffectCondition::OnEndOverlap)
			ApplyEffectToTarget(TargetActor, EffectParam, ActorLevel);

		bool NeedRemove = false;
		if (EffectParam.RemoveCondition == EEffectCondition::OnEndOverlap)
		{
			NeedRemove = true;
		}

		if (NeedRemove)
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if (!IsValid(TargetASC)) return;

			for (auto It = ActiveEffectHandles.CreateIterator(); It;)
			{
				if (It.Value() == TargetASC)
				{
					TargetASC->RemoveActiveGameplayEffect(It.Key(), 1);
					It.RemoveCurrent(); //Important: RemoveCurrent() will move Iterator to next automatically
				}
				else
				{
					++It; //Important: move Iterator to next
				}
			}
		}
	}
}
