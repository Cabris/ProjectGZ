#pragma once

#include "CoreMinimal.h"
#include "GZInventoryItemDefinition.h"
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
	
	UFUNCTION(BlueprintPure)
	int32 GetStackByTag(FGameplayTag Tag) const;
	UFUNCTION(BlueprintPure)
	TSubclassOf<UGZInventoryItemDefinition> GetItemDefinitionClass() const;
	UFUNCTION(BlueprintPure)
	UGZInventoryItemDefinition* GetItemDefinition() const;
	UFUNCTION(BlueprintPure) //use Item_Stack_Quantity
	int32 GetDefaultStack() const;
	UFUNCTION(BlueprintPure)
	bool IsEquipmentItem();

	void SetStackByTag(const FGameplayTag& Tag, int32 Stack);
	void SetItemDefinitionClass(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	TSubclassOf<UGZInventoryItemDefinition> ItemDefinitionClass;
	// UPROPERTY(Replicated)
	// FGZTagStackList TagStackList; //used for item Stack number
};
