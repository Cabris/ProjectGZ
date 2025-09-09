#include "Equipment/GZWeaponSlotComponent.h"

#include "Character/GZPawnFeatureComponent.h"
#include "Equipment/GZEquipmentManagerComponent.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGZ/ProjectGZ.h"


UGZWeaponSlotComponent::UGZWeaponSlotComponent()
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

void UGZWeaponSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, WeaponSlots);
	DOREPLIFETIME(ThisClass, ActiveWeapon);
	DOREPLIFETIME(ThisClass, ActiveWeaponSlot);
	DOREPLIFETIME(ThisClass, SlotCapacity);
}

UGZInventoryItemInstance* UGZWeaponSlotComponent::GetActiveWeaponItemInstance()
{
	return ActiveWeapon;
}

void UGZWeaponSlotComponent::AddWeaponToSlot(UGZInventoryItemInstance* Instance, int32 SlotIdx)
{
	if (!Instance || SlotIdx >= SlotCapacity)
		return;
	WeaponSlots[SlotIdx] = Instance;
	OnSlotAdded.Broadcast(Instance, SlotIdx);
}

void UGZWeaponSlotComponent::SetActiveWeaponSlot(int32 SlotIdx)
{
	if (!GetWeaponItemInstance(SlotIdx))
		return;

	if (ActiveWeaponSlot != INDEX_NONE) //Unselect old Slot
	{
		check(ActiveWeapon);
		UnequipItem(ActiveWeaponSlot);
		OnSlotUnselected.Broadcast(ActiveWeapon.Get(), SlotIdx);
		ActiveWeaponSlot = INDEX_NONE;
		ActiveWeapon = nullptr;
	}

	ActiveWeapon = WeaponSlots[SlotIdx];
	ActiveWeaponSlot = SlotIdx;
	check(ActiveWeapon);
	EquipItem(SlotIdx);
	OnSlotSelected.Broadcast(ActiveWeapon.Get(), SlotIdx);
}

void UGZWeaponSlotComponent::UnequipItem(int32 SlotIdx)
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

void UGZWeaponSlotComponent::EquipItem(int32 SlotIdx)
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

int32 UGZWeaponSlotComponent::GetActiveWeaponSlot() const
{
	return ActiveWeaponSlot;
}

int32 UGZWeaponSlotComponent::GetWeaponNum()
{
	int32 Num = 0;
	for (int i = 0; i < SlotCapacity; i++)
	{
		TObjectPtr<UGZInventoryItemInstance> Item = WeaponSlots[i];
		if (IsValid(Item))
		{
			Num += 1;
		}
	}
	return Num;
}

int32 UGZWeaponSlotComponent::GetWeaponSlot(UGZInventoryItemInstance* Instance)
{
	int32 Idx = WeaponSlots.IndexOfByPredicate([this,Instance](const TObjectPtr<UGZInventoryItemInstance>& ItemInstance)
	{
		return ItemInstance == Instance;
	});
	return Idx;
}

bool UGZWeaponSlotComponent::HasWeaponInSlots(UGZInventoryItemInstance* Instance)
{
	if (WeaponSlots.Num() == 0)return false;
	if (!IsValid(Instance))return false;
	int32 Idx = GetWeaponSlot(Instance);
	return Idx != INDEX_NONE;
}

bool UGZWeaponSlotComponent::IsSlotIdxValid(int32 SlotIdx) const
{
	return SlotIdx >= 0 && SlotIdx < SlotCapacity;
}

void UGZWeaponSlotComponent::RemoveWeaponFromSlot(int32 SlotIdx)
{
	UGZInventoryItemInstance* ItemInstance = GetWeaponItemInstance(SlotIdx);
	if (!IsValid(ItemInstance))
	{
		DEBUG_PRINTF(TEXT("UGZWeaponSlotComponent::RemoveWeaponFromSlot: ItemInstance is NULL"));
		return;
	}
	if (SlotIdx != INDEX_NONE)
	{
		WeaponSlots[SlotIdx] = nullptr;
		UnequipItem(SlotIdx);
		OnSlotRemoved.Broadcast(ItemInstance, SlotIdx);
		if (ActiveWeaponSlot == SlotIdx)
			ActiveWeaponSlot = INDEX_NONE;
	}
}

UGZInventoryItemInstance* UGZWeaponSlotComponent::GetWeaponItemInstance(int32 SlotIdx)
{
	if (SlotIdx >= SlotCapacity)return nullptr;
	return WeaponSlots[SlotIdx];
}

int32 UGZWeaponSlotComponent::FindFirstAvailableSlotIndex()
{
	return INDEX_NONE;
}
