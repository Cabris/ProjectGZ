#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GZWeaponMenuComponent.generated.h"
class UGZInventoryItemInstance;

//class for communicate with weapon menu widget controller
//will handle weapon switch/current using weapon
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTGZ_API UGZWeaponMenuComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZWeaponMenuComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UGZInventoryItemInstance* GetActiveWeaponItemInstance();
	void AddWeaponToSlot(UGZInventoryItemInstance* Instance, uint8 SlotIdx);
	void RemoveWeaponFromSlot(UGZInventoryItemInstance* Instance);
	UGZInventoryItemInstance* GetWeaponItemInstance(uint8 SlotIdx);

protected:
	UPROPERTY(Replicated)
	TArray<TObjectPtr<UGZInventoryItemInstance>> WeaponSlots;
	UPROPERTY(Replicated)
	TObjectPtr<UGZInventoryItemInstance> ActiveWeapon = nullptr;
	UPROPERTY(Replicated)
	int8 ActiveWeaponSlot = -1;
	UPROPERTY(Replicated)
	uint8 SlotCount = 3;
};
