#pragma once

#include "CoreMinimal.h"
#include "GZInventoryItemDefinition.h"
#include "AbilitySystem/Cost/GZTagStackCost.h"
#include "Equipment/GZTagStackList.h"
#include "UObject/Object.h"
#include "GZInventoryItemInstance.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PROJECTGZ_API UGZInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface


	//TagStackList
	UFUNCTION(BlueprintPure)
	int32 GetStackByTag(FGameplayTag Tag) const;
	UFUNCTION(BlueprintPure) //use Item_Stack_Quantity
	int32 GetDefaultStack() const;
	UFUNCTION(BlueprintPure)
	void SetStackByTag(const FGameplayTag& Tag, int32 Stack);
	UFUNCTION(BlueprintPure)
	bool ContainsStackTag(const FGameplayTag& StackTag);
	//End TagStackList

	UFUNCTION(BlueprintPure)
	TSubclassOf<UGZInventoryItemDefinition> GetItemDefinitionClass() const;
	UFUNCTION(BlueprintPure)
	UGZInventoryItemDefinition* GetItemDefinition() const;
	UFUNCTION(BlueprintPure)
	bool IsEquipmentItem();

	void SetItemDefinitionClass(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	TSubclassOf<UGZInventoryItemDefinition> ItemDefinitionClass;
	UPROPERTY(Replicated)
	FGZTagStackList TagStackList; //used for item Stack number
};
