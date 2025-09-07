#pragma once

#include "CoreMinimal.h"
#include "Character/GZPawnComponent.h"
#include "Components/ActorComponent.h"
#include "GZWeaponSlotComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponSlotChangedSingnature, UGZInventoryItemInstance*, Instance, int32, SlotIdx);

class UGZInventoryItemInstance;

//class for communicate with weapon menu widget controller
//will handle weapon switch/current using weapon
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTGZ_API UGZWeaponSlotComponent : public UGZPawnComponent
{
	GENERATED_BODY()

public:
	UGZWeaponSlotComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UGZInventoryItemInstance* GetActiveWeaponItemInstance();
	void AddWeaponToSlot(UGZInventoryItemInstance* Instance, uint8 SlotIdx);
	bool SetActiveWeaponSlot(uint8 SlotIdx);
	void RemoveWeaponFromSlot(UGZInventoryItemInstance* Instance);
	UGZInventoryItemInstance* GetWeaponItemInstance(uint8 SlotIdx);
	int32 FindFirstAvailableSlotIndex();
	int32 GetActiveWeaponSlot() const;
	int32 GetWeaponNum();
	
	FWeaponSlotChangedSingnature OnSlotAdded;
	FWeaponSlotChangedSingnature OnSlotRemoved;
	FWeaponSlotChangedSingnature OnSlotSelected;
	FWeaponSlotChangedSingnature OnSlotUnselected;

protected:
	UPROPERTY(Replicated)
	TArray<TObjectPtr<UGZInventoryItemInstance>> WeaponSlots;
	UPROPERTY(Replicated)
	TObjectPtr<UGZInventoryItemInstance> ActiveWeapon = nullptr;
	UPROPERTY(Replicated)
	int8 ActiveWeaponSlot = INDEX_NONE;
	UPROPERTY(Replicated)
	uint8 SlotCount = 3;
};
