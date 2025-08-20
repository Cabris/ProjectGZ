#include "Equipment/GZWeaponMenuComponent.h"

#include "Net/UnrealNetwork.h"


UGZWeaponMenuComponent::UGZWeaponMenuComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
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
	if (!Instance)return;
	WeaponSlots[SlotIdx] = Instance;
}

void UGZWeaponMenuComponent::RemoveWeaponFromSlot(UGZInventoryItemInstance* Instance)
{
	if (!Instance || !WeaponSlots.Contains(Instance))return;
	WeaponSlots.Remove(Instance);
}

UGZInventoryItemInstance* UGZWeaponMenuComponent::GetWeaponItemInstance(uint8 SlotIdx)
{
	if (SlotIdx >= SlotCount)return nullptr;
	return WeaponSlots[SlotIdx];
}
