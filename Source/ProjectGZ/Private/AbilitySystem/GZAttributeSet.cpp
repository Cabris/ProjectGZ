#include "AbilitySystem/GZAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "UObject/CoreNet.h"
#include "GameplayEffectExtension.h"

UGZAttributeSet::UGZAttributeSet()
{
	InitMaxHealth(100.f);
	InitHealth(GetMaxHealth());
}

void UGZAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UGZAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGZAttributeSet, Health, COND_None, REPNOTIFY_Always);
}

void UGZAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGZAttributeSet, MaxHealth, OldMaxHealth);
}

void UGZAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGZAttributeSet, Health, OldHealth);
}

void UGZAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UGZAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
