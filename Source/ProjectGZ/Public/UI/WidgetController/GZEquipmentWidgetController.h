#pragma once

#include "CoreMinimal.h"
#include "GZWidgetController.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "GZEquipmentWidgetController.generated.h"

USTRUCT(BlueprintType)
struct FEquipmentSlot
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasEquipment = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsActiveSlot = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGZInventoryItemInstance* Instance = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SlotIdx = INDEX_NONE;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquipmentChangedSingnature, const FEquipmentSlot&, Slot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquipmentInitialValuesSingnature, const TArray<FEquipmentSlot>&, Slots);



UCLASS()
class PROJECTGZ_API UGZEquipmentWidgetController : public UGZWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FEquipmentInitialValuesSingnature OnEquipmentInitialValues;
	UPROPERTY(BlueprintAssignable)
	FEquipmentChangedSingnature OnSlotAdded;
	UPROPERTY(BlueprintAssignable)
	FEquipmentChangedSingnature OnSlotRemoved;

protected:
private:
	UFUNCTION()
	void HandleSlotAdded(UGZInventoryItemInstance* Instance, int32 SlotIdx);
	UFUNCTION()
	void HandleSlotRemoved(UGZInventoryItemInstance* Instance, int32 SlotIdx);
	UFUNCTION()
	void HandleActiveSlotChanged(int32 NewActiveSlot);

	TArray<FEquipmentSlot> EquipmentSlots;
};
