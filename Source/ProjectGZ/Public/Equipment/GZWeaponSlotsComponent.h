#pragma once

#include "CoreMinimal.h"
#include "Character/GZPawnComponent.h"
#include "Components/ActorComponent.h"
#include "GZWeaponSlotsComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponSlotChangedSingnature, UGZInventoryItemInstance*, Instance, int32, SlotIdx);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActiveSlotChangedSingnature, int32, SlotIdx);

class UGZInventoryItemInstance;

//class for communicate with weapon menu widget controller
//will handle weapon switch/current using weapon
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTGZ_API UGZWeaponSlotsComponent : public UGZPawnComponent
{
	GENERATED_BODY()

public:
	UGZWeaponSlotsComponent();
	//
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UGZInventoryItemInstance* GetActiveWeaponItemInstance();
	//
	void AddWeaponToSlot(UGZInventoryItemInstance* Instance, int32 SlotIdx);
	void SetActiveWeaponSlot(int32 SlotIdx);
	void RemoveWeaponFromSlot(int32 SlotIdx);
	UGZInventoryItemInstance* GetWeaponItemInstance(int32 SlotIdx);
	int32 FindFirstAvailableSlotIndex();
	int32 GetActiveWeaponSlot() const;
	int32 GetSlotCapacity() const;
	bool HasWeaponInSlots(UGZInventoryItemInstance* Instance);
	bool IsSlotEmpty(int32 SlotIdx) const;
	int32 GetWeaponSlot(UGZInventoryItemInstance* Instance);
	
	//For UI Event
	UPROPERTY(BlueprintAssignable)
	FWeaponSlotChangedSingnature OnSlotAdded;
	UPROPERTY(BlueprintAssignable)
	FWeaponSlotChangedSingnature OnSlotRemoved;
	UPROPERTY(BlueprintAssignable)
	FActiveSlotChangedSingnature OnActiveSlotChanged;

protected:
	UPROPERTY(Replicated)
	TArray<TObjectPtr<UGZInventoryItemInstance>> WeaponSlots;
	UPROPERTY(Replicated)
	TObjectPtr<UGZInventoryItemInstance> ActiveWeapon = nullptr;
	UPROPERTY(Replicated)
	int32 ActiveWeaponSlot = INDEX_NONE;
	UPROPERTY(Replicated)
	int32 SlotCapacity = 3;

private:
	void UnequipItem(int32 SlotIdx);
	void EquipItem(int32 SlotIdx);
};
