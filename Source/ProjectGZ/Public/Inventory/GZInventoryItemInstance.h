#pragma once

#include "CoreMinimal.h"
#include "GZInventoryItemDefinition.h"
#include "Equipment/GZTagStackList.h"
#include "UObject/Object.h"
#include "GZInventoryItemInstance.generated.h"

UCLASS()
class PROJECTGZ_API UGZInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	uint32 GetStackByTag(const FGameplayTag& Tag) const;
	void SetStackByTag(const FGameplayTag& Tag, int32 Stack);
	const TSubclassOf<UGZInventoryItemDefinition>& GetItemDefinitionClass() const;
	void SetItemDefinitionClass(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	TSubclassOf<UGZInventoryItemDefinition> ItemDefinitionClass;
	UPROPERTY(Replicated)
	FGZTagStackList TagStackList;//used for item Stack number
};
