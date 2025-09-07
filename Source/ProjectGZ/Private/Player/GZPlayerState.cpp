#include "Player/GZPlayerState.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "AbilitySystem/GZAttributeSet.h"
#include "Interfactions/GZCombatInterface.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGZ/ProjectGZ.h"

AGZPlayerState::AGZPlayerState()
{
	SetNetUpdateFrequency(100.f);
	AttributeSet = CreateDefaultSubobject<UGZAttributeSet>("AttributeSet");
	AbilitySystemComponent = CreateDefaultSubobject<UGZAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	bReplicates = true;
}

void AGZPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, CurrentEquipmentTag);
}

UGZAbilitySystemComponent* AGZPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UGZAttributeSet* AGZPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AGZPlayerState::UpdateCurrentEquipmentTag(FGameplayTag NewEquipmentTag)
{
	CurrentEquipmentTag = NewEquipmentTag;
}

void AGZPlayerState::OnRep_CurrentEquipmentTag(const FGameplayTag& OldEquipmentTag)
{
	auto Pawn = GetPawn();
	check(Pawn);
	if (Pawn->Implements<UGZCombatInterface>())
	{
		IGZCombatInterface::Execute_UpdateEquipmentTag(Pawn, CurrentEquipmentTag);
	}
	else
	{
		Debug::Print(TEXT("OnWeaponSlotSelected: Pawn not Implements UGZCombatInterface"));
	}
}
