#include "Character/GZPawnFeatureComponent.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "Equipment/GZEquipmentDefinition.h"
#include "Equipment/GZEquipmentManagerComponent.h"
#include "Equipment/GZWeaponMenuComponent.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Equipment/GZEquipmentInstance.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/Pawn.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGZ/ProjectGZ.h"

UGZPawnFeatureComponent::UGZPawnFeatureComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
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

	if (ItemInstance->IsEquipmentItem())
	{
		//try to equip item
		TryGrantEquipmentToPawn(ItemInstance);
	}
	return true;
}

void UGZPawnFeatureComponent::TryGrantEquipmentToPawn(UGZInventoryItemInstance* ItemInstance)
{
	TSubclassOf<UGZEquipmentDefinition> EquipmentDefClass = ItemInstance->GetItemDefinition()->GetEquipmentDef();
	if (IsValid(EquipmentDefClass)) //can be equipped 
	{
		UGZEquipmentDefinition* EquipmentDef = EquipmentDefClass.GetDefaultObject();
		if (!GetEquipmentManager() || !EquipmentDef || !IsValid(EquipmentDef->InstanceClass))
		{
			Debug::Print(TEXT("Failed to TryGrantEquipmentToPawn: EquipmentDef is NULL"));
			return;
		}

		UGZEquipmentInstance* OwningWeaponInstance = GetEquipmentManager()->GetEquipmentInstanceByClass(EquipmentDef->InstanceClass);
		if (!OwningWeaponInstance)
		{
			//add weapon to Equipment
			UGZEquipmentInstance* NewWeaponInstance = GetEquipmentManager()->EquipItem(ItemInstance);
			if (!IsValid(NewWeaponInstance))
			{
				Debug::Print(TEXT("Failed to TryGrantEquipmentToPawn: NewWeaponInstance is NULL"));
				return;
			}
			TObjectPtr<UGZWeaponMenuComponent> WeaponMenu = GetWeaponMenu();
			if (WeaponMenu)
				WeaponMenu->AddWeaponToSlot(ItemInstance, 0);
		}
		else
		{
			Debug::Print(TEXT("TODO: add ammo to Weapon Instance"));
			//TODO: add ammo to Weapon Instance
			//ItemInstance->SetStackByTag("Ammo",100);
		}

	}
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


