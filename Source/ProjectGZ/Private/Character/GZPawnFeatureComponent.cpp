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

//User PICKUP an equipment
bool UGZPawnFeatureComponent::TryGrantEquipmentToPawn(UGZInventoryItemInstance* ItemInstance)
{
	if (!CheckComponents())return false;
	UGZWeaponSlotComponent* WeaponSlotManager = GetWeaponSlotManager();
	//Check Slot has space
	int32 Idx = WeaponSlotManager->FindFirstAvailableSlotIndex();
	if (Idx == INDEX_NONE)
	{
		DEBUG_PRINTF(TEXT("Failed to TryGrantEquipmentToPawn: WeaponSlot is Full"));
		return false;
	}

	if (!WeaponSlotManager->HasWeaponInSlots(ItemInstance))
	{
		//Add to slot
		WeaponSlotManager->AddWeaponToSlot(ItemInstance, Idx);
		WeaponSlotManager->SetActiveWeaponSlot(Idx);
	}
	return true;
}

//User DROP an equipment
bool UGZPawnFeatureComponent::TryRemoveEquipmentFromPawn(UGZInventoryItemInstance* ItemInstance)
{
	UGZEquipmentDefinition* EquipmentDef = UGZEquipmentDefinition::GetDefinition(ItemInstance);

	if (!IsValid(EquipmentDef))
	{
		DEBUG_PRINTF(TEXT("Failed to TryRemoveEquipmentFromPawn: EquipmentDef is NULL"));
		return false;
	}
	if (!CheckComponents())return false;
	UGZWeaponSlotComponent* WeaponSlotManager = GetWeaponSlotManager();
	//Remove from slot
	int32 SlotIdx = WeaponSlotManager->GetWeaponSlot(ItemInstance);
	if (SlotIdx == INDEX_NONE)
	{
		DEBUG_PRINTF(TEXT("Failed to TryRemoveEquipmentFromPawn: GetWeaponSlot is INDEX_NONE"));
		return false;
	}
	WeaponSlotManager->RemoveWeaponFromSlot(SlotIdx);
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

bool UGZPawnFeatureComponent::CheckComponents()
{
	UGZEquipmentManagerComponent* EquipmentManager = GetEquipmentManager();
	if (!IsValid(EquipmentManager))
	{
		DEBUG_PRINTF(TEXT("Failed to TryRemoveEquipmentFromPawn: EquipmentManager is NULL"));
		return false;
	}
	UGZWeaponSlotComponent* WeaponSlot = GetWeaponSlotManager();
	if (!IsValid(WeaponSlot))
	{
		DEBUG_PRINTF(TEXT("Failed to TryRemoveEquipmentFromPawn: WeaponSlot is NULL"));
		return false;
	}
	return true;
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

UGZWeaponSlotComponent* UGZPawnFeatureComponent::GetWeaponSlotManager()
{
	return PawnFeatureStruct.WeaponMenu;
}

APlayerController* UGZPawnFeatureComponent::GetPlayerController()
{
	return PawnFeatureStruct.PlayerController;
}
