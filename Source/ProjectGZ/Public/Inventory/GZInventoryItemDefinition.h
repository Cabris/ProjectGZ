#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GZInventoryItemDefinition.generated.h"


class UGZEquipmentDefinition;
class UGZInventoryItemInstance;

UCLASS(Blueprintable,BlueprintType)
class PROJECTGZ_API UGZInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	const TObjectPtr<UTexture2D>& GetItemIcon() const;
	const TSubclassOf<UGZEquipmentDefinition>& GetEquipmentDef() const;
	const FName& GetItemName() const;
	const TSubclassOf<UGZInventoryItemInstance>& GetItemInstanceeDef() const;
	const TMap<FGameplayTag, uint32>& GetItemTagStacks() const;
	const TObjectPtr<UStaticMesh>& GetPickupMesh() const;
	const FTransform& GetPickupMeshOffsetTransform() const
	{
		return PickupMeshOffsetTransform;
	}
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|World Pickup")
	TObjectPtr<UStaticMesh> PickupMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|World Pickup")
	FTransform PickupMeshOffsetTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Item")
	TObjectPtr<UTexture2D> ItemIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Item")
	FName ItemName;
	UPROPERTY(EditDefaultsOnly, Category="Inventory|Item")
	TSubclassOf<UGZInventoryItemInstance> InstanceDef;
	UPROPERTY(EditDefaultsOnly, Category="Inventory|Item")
	TMap<FGameplayTag, uint32> ItemTagStacks;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Equipment")
	TSubclassOf<UGZEquipmentDefinition> EquipmentDef;
};
