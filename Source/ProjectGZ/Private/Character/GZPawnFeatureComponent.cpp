#include "Character/GZPawnFeatureComponent.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "Equipment/GZEquipmentDefinition.h"
#include "Equipment/GZEquipmentManagerComponent.h"
#include "Equipment/GZWeaponMenuComponent.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Equipment/GZWeaponInstance.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/Pawn.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

UGZPawnFeatureComponent::UGZPawnFeatureComponent()
{
}

void UGZPawnFeatureComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	GetAbilitySystem()->InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}

bool UGZPawnFeatureComponent::TryGrantItemToPawn(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefinitionClass, APawn* ReceivingPawn)
{
	if (!GetInventoryManager()) return false;
	UGZInventoryItemInstance* ItemInstance = GetInventoryManager()->AddItemDefToInventory(ItemDefinitionClass);
	if (!ItemInstance) return false;

	UGZInventoryItemDefinition* ItemDef = ItemDefinitionClass.GetDefaultObject();

	//try to equip item
	TSubclassOf<UGZEquipmentDefinition> EquipmentDefClass = ItemDef->GetEquipmentDef();
	if (IsValid(EquipmentDefClass)) //can be equipped 
	{
		UGZEquipmentDefinition* EquipmentDef = EquipmentDefClass.GetDefaultObject();
		if (!GetEquipmentManager() || !EquipmentDef || !IsValid(EquipmentDef->InstanceClass))return false;

		UGZWeaponInstance* OwningWeaponInstance = GetEquipmentManager()->GetEquipmentInstanceByClass(EquipmentDef->InstanceClass);
		if (!OwningWeaponInstance)
		{
			//add weapon to Equipment
			UGZWeaponInstance* NewWeaponInstance = GetEquipmentManager()->EquipItem(EquipmentDefClass);
			if (!IsValid(NewWeaponInstance))return false;
			TObjectPtr<UGZWeaponMenuComponent> WeaponMenu = GetWeaponMenu();
			if (WeaponMenu)
				WeaponMenu->AddWeaponToSlot(ItemInstance, 0);
		}
		//TODO: add ammo to Weapon Instance
		//ItemInstance->SetStackByTag("Ammo",100);
	}
	return true;
}


void UGZPawnFeatureComponent::OnInitializePawnFeature()
{
	GetAbilitySystem()->OnAbilityActorInfoSet();
}

void UGZPawnFeatureComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, PawnFeatureStruct);
}
