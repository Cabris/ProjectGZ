#include "Player/GZPlayerState.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "AbilitySystem/GZAttributeSet.h"


AGZPlayerState::AGZPlayerState()
{
	SetNetUpdateFrequency(100.f);
	AbilitySystemComponent=CreateDefaultSubobject<UGZAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet=CreateDefaultSubobject<UGZAttributeSet>("AttributeSet");
}

