#pragma once
#include "CoreMinimal.h"
#include "AbilitySystem/Ability/GZGameplayAbility.h"
#include "GZCombatAbility.generated.h"

class UGZEquipmentInstance;
class UGZInventoryItemInstance;

UCLASS()
class PROJECTGZ_API UGZCombatAbility : public UGZGameplayAbility
{
	GENERATED_BODY()

public:
	UGZCombatAbility();

protected:
	//Get the associated Inventory item instance: ammo quantity stat
	UGZInventoryItemInstance* GetItemInstance() const;

	//Get the associated weapon instance for weapon attribute stat: damage range, gravity effect factor, projectile attributes
	UGZEquipmentInstance* GetEquipmentInstance() const;

private:
};
