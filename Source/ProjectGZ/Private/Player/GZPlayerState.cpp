#include "Player/GZPlayerState.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "AbilitySystem/GZAttributeSet.h"
#include "Character//GZPawnFeatureComponent.h"
#include "Equipment/GZEquipmentManagerComponent.h"
#include "Equipment/GZWeaponMenuComponent.h"
#include "Inventory/GZInventoryManagerComponent.h"


AGZPlayerState::AGZPlayerState()
{
	SetNetUpdateFrequency(100.f);
	AttributeSet = CreateDefaultSubobject<UGZAttributeSet>("AttributeSet");
	AbilitySystemComponent = CreateDefaultSubobject<UGZAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	InventoryManager = CreateDefaultSubobject<UGZInventoryManagerComponent>("InventoryManagerComponent");
	EquipmentManager = CreateDefaultSubobject<UGZEquipmentManagerComponent>("EquipmentManagerComponent");
	WeaponMenu = CreateDefaultSubobject<UGZWeaponMenuComponent>("WeaponMenuComponent");

	FPawnFeatureStruct PawnFeatureStruct;
	PawnFeatureStruct.AbilitySystemComponent = AbilitySystemComponent;
	PawnFeatureStruct.AttributeSet = AttributeSet;
	PawnFeatureStruct.InventoryManager = InventoryManager;
	PawnFeatureStruct.EquipmentManager = EquipmentManager;
	PawnFeatureStruct.WeaponMenu = WeaponMenu;
	PawnFeature = CreateDefaultSubobject<UGZPawnFeatureComponent>("PawnFeatureComponent");
	PawnFeature->SetupPawnFeature(PawnFeatureStruct);
	OnPawnSet.AddDynamic(this, &ThisClass::OnControlledPawnSet);
}

void AGZPlayerState::OnControlledPawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	PawnFeature->SetControlledPawn(NewPawn);
}
