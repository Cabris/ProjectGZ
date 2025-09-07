#include "Character/GZPawnFeatureComponent.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "Equipment/GZEquipmentDefinition.h"
#include "Equipment/GZEquipmentManagerComponent.h"
#include "Equipment/GZWeaponSlotComponent.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Equipment/GZEquipmentInstance.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/Pawn.h"
#include "Interfactions/GZCombatInterface.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Player/GZPlayerController.h"
#include "Player/GZPlayerState.h"
#include "ProjectGZ/ProjectGZ.h"

UGZPawnFeatureComponent* UGZPawnFeatureComponent::Get(AActor* Target)
{
	if (!IsValid(Target)) return nullptr;
	IGZPawnFeatureInterface* PawnInterface = Cast<IGZPawnFeatureInterface>(Target);
	if (!PawnInterface) return nullptr;
	return PawnInterface->GetPawnFeature();
}

UGZPawnFeatureComponent::UGZPawnFeatureComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UGZPawnFeatureComponent::TryGrantItemToPawn(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefinitionClass,
                                                 APawn* ReceivingPawn)
{
	if (!GetInventoryManager()) return false;
	UGZInventoryItemInstance* ItemInstance = GetInventoryManager()->AddItemDefToInventory(ItemDefinitionClass);
	if (!ItemInstance) return false;
	return true;
}

bool UGZPawnFeatureComponent::TryGrantEquipmentToPawn(UGZInventoryItemInstance* ItemInstance)
{
	if (!IsValid(ItemInstance)) return false;
	UGZInventoryItemDefinition* ItemDefinition = ItemInstance->GetItemDefinition();
	if (!IsValid(ItemDefinition)) return false;
	TSubclassOf<UGZEquipmentDefinition> EquipmentDefClass = ItemDefinition->GetEquipmentDef();
	if (!IsValid(EquipmentDefClass)) return false;

	//Check Slot has space
	UGZWeaponSlotComponent* const& WeaponSlot = GetWeaponSlot();
	if (!IsValid(WeaponSlot))return false;
	
	int32 Idx=WeaponSlot->FindFirstAvailableSlotIndex();
	if (Idx == INDEX_NONE)
	{
		DEBUG_PRINTF(TEXT("Failed to TryGrantEquipmentToPawn: WeaponSlot is Full"));
		return false;
	}
	WeaponSlot->AddWeaponToSlot(ItemInstance, Idx);
 
	
	//can be equipped
	UGZEquipmentDefinition* EquipmentDef = EquipmentDefClass.GetDefaultObject();
	UGZEquipmentManagerComponent* EquipmentManager = GetEquipmentManager();
	if (!EquipmentManager || !EquipmentDef || !IsValid(EquipmentDef->InstanceClass))
	{
		DEBUG_PRINTF(TEXT("Failed to TryGrantEquipmentToPawn: EquipmentDef is NULL"));
		return false;
	}

	UGZEquipmentInstance* OwningWeaponInstance = EquipmentManager->GetEquipmentInstanceByClass(EquipmentDef->InstanceClass);
	if (!OwningWeaponInstance)
	{
		//add weapon to Equipment
		UGZEquipmentInstance* NewWeaponInstance = EquipmentManager->EquipItem(ItemInstance);
		if (!IsValid(NewWeaponInstance))
		{
			Debug::Print(TEXT("Failed to TryGrantEquipmentToPawn: NewWeaponInstance is NULL"));
			return false;
		}
	}


	return true;
}

void UGZPawnFeatureComponent::InitializePawnFeature(AGZPlayerController* GZPlayerController, AGZPlayerState* GZPlayerState, APawn* Pawn)
{
	FPawnFeatureStruct FeatureStruct;
	FeatureStruct.Pawn = Pawn;

	if (IsValid(GZPlayerState))
	{
		FeatureStruct.PlayerState = GZPlayerState;
		FeatureStruct.AbilitySystem = GZPlayerState->GetAbilitySystemComponent();
	}

	if (IsValid(GZPlayerController))
	{
		FeatureStruct.PlayerController = GZPlayerController;
		FeatureStruct.InventoryManager = GZPlayerController->GetComponentByClass<UGZInventoryManagerComponent>();
		FeatureStruct.EquipmentManager = GZPlayerController->GetComponentByClass<UGZEquipmentManagerComponent>();
		FeatureStruct.WeaponMenu = GZPlayerController->GetComponentByClass<UGZWeaponSlotComponent>();
		FeatureStruct.WeaponMenu->OnSlotSelected.AddDynamic(this, &ThisClass::OnWeaponSlotSelected);
	}
	bool bHasAuthority = GZPlayerController->HasAuthority(); //HasAuthority false
	FeatureStruct.HasAuthority = bHasAuthority ? 1 : 0;
	PawnFeatureStruct = FeatureStruct;
}

void UGZPawnFeatureComponent::OnWeaponSlotSelected(UGZInventoryItemInstance* Instance, int SlotIdx)
{
	if (!IsValid(Instance))return;
	if (!Instance->IsEquipmentItem())return;
	TSubclassOf<UGZEquipmentDefinition> EquipmentDefClass = Instance->GetItemDefinition()->GetEquipmentDef();
	UGZEquipmentDefinition* EquipmentDef = EquipmentDefClass.GetDefaultObject();
	if (!IsValid(EquipmentDef))return;
	AGZPlayerState* PS = Cast<AGZPlayerState>(PawnFeatureStruct.PlayerState);
	if (PS)
	{
		PS->UpdateCurrentEquipmentTag(EquipmentDef->EquipmentAnimLayerTag);
	}
}

APawn* UGZPawnFeatureComponent::GetPawn()
{
	return PawnFeatureStruct.Pawn;
}

UGZAbilitySystemComponent* UGZPawnFeatureComponent::GetAbilitySystem()
{
	return PawnFeatureStruct.AbilitySystem;
}

UGZInventoryManagerComponent* UGZPawnFeatureComponent::GetInventoryManager()
{
	return PawnFeatureStruct.InventoryManager;
}

UGZEquipmentManagerComponent* UGZPawnFeatureComponent::GetEquipmentManager()
{
	return PawnFeatureStruct.EquipmentManager;
}

UGZWeaponSlotComponent* UGZPawnFeatureComponent::GetWeaponSlot()
{
	return PawnFeatureStruct.WeaponMenu;
}

APlayerController* UGZPawnFeatureComponent::GetPlayerController()
{
	return PawnFeatureStruct.PlayerController;
}
