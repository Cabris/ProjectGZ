#pragma once
#include "CoreMinimal.h"
#include "AbilitySystem/Ability/GZGameplayAbility.h"
#include "GZCombatAbility.generated.h"

class UGZDamageEffect;
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
	UFUNCTION(BlueprintPure, Category="Ability|Combat")
	UGZInventoryItemInstance* GetItemInstance() const;

	//Get the associated weapon instance for weapon attribute stat: damage range, gravity effect factor, projectile attributes
	UFUNCTION(BlueprintPure, Category="Ability|Combat")
	UGZEquipmentInstance* GetEquipmentInstance() const;

	UPROPERTY(EditDefaultsOnly, Category="Ability|Combat")
	float AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, Category="Ability|Combat")
	TSubclassOf<UGZDamageEffect> DamageEffectClass;
private:

};
