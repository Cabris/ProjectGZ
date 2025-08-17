#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GZInventoryItemDefinition.generated.h"


class UGZEquipmentDefinition;
class UGZInventoryItemInstance;

USTRUCT(BlueprintType)
struct FGameplayTagStackEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;
};

UCLASS(Blueprintable, BlueprintType)
class PROJECTGZ_API UGZInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Inventory|Item")
	TSubclassOf<UGZEquipmentDefinition> GetEquipmentDef() const
	{
		return EquipmentDef;
	}

	UFUNCTION(BlueprintPure, Category="Inventory|Item")
	TSubclassOf<UGZInventoryItemInstance> GetItemInstanceeDef() const
	{
		return InstanceDef;
	}

	UFUNCTION(BlueprintPure, Category="Inventory|Item")
	FName GetItemName() const
	{
		return ItemName;
	}

	UFUNCTION(BlueprintPure, Category="Inventory|Item")
	int32 GetTagStackCount(FGameplayTag Tag) const
	{
		for (const FGameplayTagStackEntry& Entry : ItemTagStacks)
		{
			if (Entry.Tag == Tag)
				return Entry.Count;
		}
		return 0;
	}


	UFUNCTION(BlueprintPure, Category="Inventory|Item")
	TSoftObjectPtr<UStaticMesh> GetPickupMeshSoftPtr() const
	{
		return PickupMesh;
	}

	UFUNCTION(BlueprintPure, Category="Inventory|Item")
	UTexture2D* GetItemIcon() const
	{
		return ItemIcon;
	}

	UFUNCTION(BlueprintPure, Category="Inventory|Item")
	FTransform GetPickupMeshOffsetTransform() const
	{
		return PickupMeshOffsetTransform;
	}

	UFUNCTION(BlueprintPure, Category="Inventory|Item")
	TArray<FGameplayTagStackEntry> GetItemTagStacks() const
	{
		return ItemTagStacks;
	}

 	FORCEINLINE const TArray<FGameplayTagStackEntry>& GetItemTagStacksRef() const
	{
		return ItemTagStacks;
	}

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|World Pickup")
	TSoftObjectPtr<UStaticMesh> PickupMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|World Pickup")
	FTransform PickupMeshOffsetTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Item")
	TObjectPtr<UTexture2D> ItemIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Item")
	FName ItemName;
	UPROPERTY(EditDefaultsOnly, Category="Inventory|Item")
	TSubclassOf<UGZInventoryItemInstance> InstanceDef;
	UPROPERTY(EditDefaultsOnly, Category="Inventory|Item")
	TArray<FGameplayTagStackEntry> ItemTagStacks;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Equipment")
	TSubclassOf<UGZEquipmentDefinition> EquipmentDef;
};
