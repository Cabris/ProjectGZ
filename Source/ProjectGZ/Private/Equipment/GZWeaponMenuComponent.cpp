#include "Equipment/GZWeaponMenuComponent.h"

#include "Net/UnrealNetwork.h"


UGZWeaponMenuComponent::UGZWeaponMenuComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	WeaponSlots.Reserve(SlotCount);
	WeaponSlots.SetNum(SlotCount);
	for (int i = 0; i < SlotCount; i++)
	{
		WeaponSlots[i] = nullptr;
	}
}

void UGZWeaponMenuComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, WeaponSlots);
	DOREPLIFETIME(ThisClass, ActiveWeapon);
	DOREPLIFETIME(ThisClass, ActiveWeaponSlot);
	DOREPLIFETIME(ThisClass, SlotCount);
}

UGZInventoryItemInstance* UGZWeaponMenuComponent::GetActiveWeaponItemInstance()
{
	return ActiveWeapon;
}

void UGZWeaponMenuComponent::AddWeaponToSlot(UGZInventoryItemInstance* Instance, uint8 SlotIdx)
{
	if (!Instance || SlotIdx >= SlotCount)
		return;
	WeaponSlots[SlotIdx] = Instance;
	OnSlotAdded.Broadcast(Instance, SlotIdx);
}

bool UGZWeaponMenuComponent::SetActiveWeaponSlot(uint8 SlotIdx)
{
	if (SlotIdx >= WeaponSlots.Num())return false;
	if (GetWeaponItemInstance(SlotIdx) == nullptr)return false;

	if (ActiveWeaponSlot != INDEX_NONE) //Unselect old Slot
	{
		check(ActiveWeapon);
		OnSlotUnselected.Broadcast(ActiveWeapon.Get(), SlotIdx);
		ActiveWeaponSlot = INDEX_NONE;
		ActiveWeapon = nullptr;
	}

	ActiveWeapon = WeaponSlots[SlotIdx];
	ActiveWeaponSlot = SlotIdx;
	check(ActiveWeapon);
	OnSlotSelected.Broadcast(ActiveWeapon.Get(), SlotIdx);
	return true;
}

void UGZWeaponMenuComponent::RemoveWeaponFromSlot(UGZInventoryItemInstance* Instance)
{
	if (!Instance || !WeaponSlots.Contains(Instance))return;
	int32 Idx = WeaponSlots.IndexOfByKey(Instance);
	if (Idx != INDEX_NONE)
	{
		WeaponSlots[Idx] = nullptr;
		OnSlotRemoved.Broadcast(Instance, Idx);
	}
}

UGZInventoryItemInstance* UGZWeaponMenuComponent::GetWeaponItemInstance(uint8 SlotIdx)
{
	if (SlotIdx >= SlotCount)return nullptr;
	return WeaponSlots[SlotIdx];
}
