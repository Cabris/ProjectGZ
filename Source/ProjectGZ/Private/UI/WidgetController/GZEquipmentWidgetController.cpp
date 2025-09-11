#include "UI/WidgetController/GZEquipmentWidgetController.h"
#include "Character/GZPawnFeatureComponent.h"
#include "Equipment/GZWeaponSlotsComponent.h"

void UGZEquipmentWidgetController::BindCallbacksToDependencies()
{
	//UGZEquipmentManagerComponent* EquipmentManager = PawnFeatureComponent->GetEquipmentManager();
	UGZWeaponSlotsComponent* WeaponSlots = PawnFeatureComponent->GetWeaponSlotsManager();
	WeaponSlots->OnSlotAdded.AddDynamic(this, &ThisClass::HandleSlotAdded);
	WeaponSlots->OnSlotRemoved.AddDynamic(this, &ThisClass::HandleSlotRemoved);
	WeaponSlots->OnActiveSlotChanged.AddDynamic(this, &ThisClass::HandleActiveSlotChanged);
	// WeaponSlots->OnSlotUnselected.AddDynamic(this, &ThisClass::HandleSlotUnselected);
}

void UGZEquipmentWidgetController::BroadcastInitialValues()
{
	UGZWeaponSlotsComponent* WeaponSlots = PawnFeatureComponent->GetWeaponSlotsManager();
	int32 SlotCapacity = WeaponSlots->GetSlotCapacity();
	EquipmentSlots.Reserve(SlotCapacity);
	EquipmentSlots.SetNum(SlotCapacity);
	for (int32 SlotIdx = 0; SlotIdx < SlotCapacity; SlotIdx++)
	{
		UGZInventoryItemInstance* Instance = WeaponSlots->GetWeaponItemInstance(SlotIdx);
		EquipmentSlots[SlotIdx].SlotIdx = SlotIdx;
		EquipmentSlots[SlotIdx].IsActiveSlot = (SlotIdx == WeaponSlots->GetActiveWeaponSlot());
		if (IsValid(Instance))
		{
			EquipmentSlots[SlotIdx].HasEquipment = true;
			EquipmentSlots[SlotIdx].Instance = Instance;
		}
		else
		{
			EquipmentSlots[SlotIdx].HasEquipment = false;
			EquipmentSlots[SlotIdx].Instance = nullptr;
		}
	}
	OnEquipmentInitialValues.Broadcast(EquipmentSlots);
}

void UGZEquipmentWidgetController::HandleSlotAdded(UGZInventoryItemInstance* Instance, int32 SlotIdx)
{
	EquipmentSlots[SlotIdx].HasEquipment = true;
	EquipmentSlots[SlotIdx].Instance = Instance;
	OnSlotAdded.Broadcast(EquipmentSlots[SlotIdx]);
}

void UGZEquipmentWidgetController::HandleSlotRemoved(UGZInventoryItemInstance* Instance, int32 SlotIdx)
{
	OnSlotRemoved.Broadcast(EquipmentSlots[SlotIdx]);
	EquipmentSlots[SlotIdx].HasEquipment = false;
	EquipmentSlots[SlotIdx].Instance = nullptr;
}

void UGZEquipmentWidgetController::HandleActiveSlotChanged(int32 NewActiveSlot)
{
	UGZWeaponSlotsComponent* WeaponSlots = PawnFeatureComponent->GetWeaponSlotsManager();
	int32 SlotCapacity = WeaponSlots->GetSlotCapacity();
	for (int32 SlotIdx = 0; SlotIdx < SlotCapacity; SlotIdx++)
	{
		EquipmentSlots[SlotIdx].IsActiveSlot = (SlotIdx == NewActiveSlot);
	}
}
