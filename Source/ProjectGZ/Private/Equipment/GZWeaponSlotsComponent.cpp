#include "Equipment/GZWeaponSlotsComponent.h"

#include "Character/GZPawnFeatureComponent.h"
#include "Equipment/GZEquipmentManagerComponent.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGZ/ProjectGZ.h"


UGZWeaponSlotsComponent::UGZWeaponSlotsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	WeaponSlots.Reserve(SlotCapacity);
	WeaponSlots.SetNum(SlotCapacity);
	for (int i = 0; i < SlotCapacity; i++)
	{
		WeaponSlots[i] = nullptr;
	}
}

void UGZWeaponSlotsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, WeaponSlots);
	DOREPLIFETIME(ThisClass, ActiveWeapon);
	DOREPLIFETIME(ThisClass, ActiveWeaponSlot);
	DOREPLIFETIME(ThisClass, SlotCapacity);
}

UGZInventoryItemInstance* UGZWeaponSlotsComponent::GetActiveWeaponItemInstance()
{
	return ActiveWeapon;
}

void UGZWeaponSlotsComponent::AddWeaponToSlot(UGZInventoryItemInstance* Instance, int32 SlotIdx)
{
	if (!Instance || SlotIdx >= SlotCapacity)
		return;
	WeaponSlots[SlotIdx] = Instance;
	OnSlotAdded.Broadcast(Instance, SlotIdx);
}

void UGZWeaponSlotsComponent::SetActiveWeaponSlot(int32 SlotIdx)
{
	if (!GetWeaponItemInstance(SlotIdx))
		return;

	if (ActiveWeaponSlot != INDEX_NONE) //UnequipItem old Slot
	{
		check(ActiveWeapon);
		UnequipItem(ActiveWeaponSlot);
		ActiveWeaponSlot = INDEX_NONE;
		ActiveWeapon = nullptr;
	}

	ActiveWeapon = WeaponSlots[SlotIdx];
	ActiveWeaponSlot = SlotIdx;
	OnActiveSlotChanged.Broadcast(ActiveWeaponSlot);
	check(ActiveWeapon);
	EquipItem(SlotIdx);
}

void UGZWeaponSlotsComponent::RemoveWeaponFromSlot(int32 SlotIdx)
{
	UGZInventoryItemInstance* ItemInstance = GetWeaponItemInstance(SlotIdx);
	if (!IsValid(ItemInstance))
	{
		DEBUG_PRINTF(TEXT("UGZWeaponSlotComponent::RemoveWeaponFromSlot: ItemInstance is NULL"));
		return;
	}
	if (SlotIdx != INDEX_NONE)
	{
		//Unequip Slot
		WeaponSlots[SlotIdx] = nullptr;
		UnequipItem(SlotIdx);
		OnSlotRemoved.Broadcast(ItemInstance, SlotIdx);
		//if is current using Slot
		if (ActiveWeaponSlot == SlotIdx)
		{
			ActiveWeaponSlot = INDEX_NONE;
			OnActiveSlotChanged.Broadcast(ActiveWeaponSlot);
		}
	}
}

void UGZWeaponSlotsComponent::UnequipItem(int32 SlotIdx)
{
	UGZInventoryItemInstance* ItemInstance = GetWeaponItemInstance(SlotIdx);
	if (!IsValid(ItemInstance))
	{
		DEBUG_PRINTF(TEXT("UGZWeaponSlotComponent::UnequipItem: ItemInstance is NULL"));
		return;
	}
	UGZEquipmentManagerComponent* EquipmentManager = GetPawnFeature()->GetEquipmentManager();
	auto WeaponInstance = EquipmentManager->GetInstanceByItem(ItemInstance);
	if (!WeaponInstance)
	{
		DEBUG_PRINTF(TEXT("UGZWeaponSlotComponent::UnequipItem: WeaponInstance is NULL"));
		return;
	}
	EquipmentManager->UnEquip(WeaponInstance);
}

void UGZWeaponSlotsComponent::EquipItem(int32 SlotIdx)
{
	UGZInventoryItemInstance* ItemInstance = GetWeaponItemInstance(SlotIdx);
	if (!IsValid(ItemInstance))
	{
		DEBUG_PRINTF(TEXT("UGZWeaponSlotComponent::EquipItem: ItemInstance is NULL"));
		return;
	}

	UGZEquipmentDefinition* EquipmentDef = UGZEquipmentDefinition::GetDefinition(ItemInstance);
	if (!IsValid(EquipmentDef))
	{
		DEBUG_PRINTF(TEXT("UGZWeaponSlotComponent::EquipItem: EquipmentDef is NULL"));
		return;
	}

	UGZEquipmentManagerComponent* EquipmentManager = GetPawnFeature()->GetEquipmentManager();
	//Attach Equipment to Pawn
	if (!EquipmentManager->HasInstanceByClass(EquipmentDef->InstanceClass))
	{
		//Create EquipmentInstance and Attach to Pawn
		UGZEquipmentInstance* NewWeaponInstance = EquipmentManager->EquipItem(ItemInstance);
		if (!IsValid(NewWeaponInstance))
		{
			Debug::Print(TEXT("UGZWeaponSlotComponent::EquipItem: EquipItem Fail"));
			return;
		}
	}
}

int32 UGZWeaponSlotsComponent::GetActiveWeaponSlot() const
{
	return ActiveWeaponSlot;
}

int32 UGZWeaponSlotsComponent::GetSlotCapacity() const
{
	return SlotCapacity;
}

int32 UGZWeaponSlotsComponent::GetWeaponSlot(UGZInventoryItemInstance* Instance)
{
	int32 Idx = WeaponSlots.IndexOfByPredicate([this,Instance](const TObjectPtr<UGZInventoryItemInstance>& ItemInstance)
	{
		return ItemInstance == Instance;
	});
	return Idx;
}

bool UGZWeaponSlotsComponent::HasWeaponInSlots(UGZInventoryItemInstance* Instance)
{
	if (WeaponSlots.Num() == 0)return false;
	if (!IsValid(Instance))return false;
	int32 Idx = GetWeaponSlot(Instance);
	return Idx != INDEX_NONE;
}

bool UGZWeaponSlotsComponent::IsSlotEmpty(int32 SlotIdx) const
{
	if (SlotIdx < 0 || SlotIdx >= SlotCapacity)return false;
	return WeaponSlots[SlotIdx] == nullptr;
}


UGZInventoryItemInstance* UGZWeaponSlotsComponent::GetWeaponItemInstance(int32 SlotIdx)
{
	if (SlotIdx >= SlotCapacity)return nullptr;
	return WeaponSlots[SlotIdx];
}

int32 UGZWeaponSlotsComponent::FindFirstAvailableSlotIndex()
{
	for (int32 Idx = 0; Idx < SlotCapacity; Idx++)
	{
		if (IsSlotEmpty(Idx))
			return Idx;
	}
	return INDEX_NONE;
}
