#pragma once
#include "CoreMinimal.h"
#include "GZTagStackList.h"
#include "UObject/Object.h"
#include "GZEquipmentInstance.generated.h"

class UGZEquipmentDefinition;
class UGZInventoryItemInstance;
class FLifetimeProperty;
struct FGameplayAbilitySpecHandle;
struct FGZEquipmentActorToSpawn;

//base class for weapon instance that used by player
//data stored in a weapon instance should only be its static attributes, ex: damage, range, auto fire rate,...
//a weapon instance should focus on its attacking useage
//data such as ammo that will change in gameplay should be stored in inventory instance
UCLASS(Blueprintable, BlueprintType)
class PROJECTGZ_API UGZEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface
	virtual void OnEquipped();
	virtual void OnUnequipped();
	virtual void SpawnEquipmentActors(TArray<FGZEquipmentActorToSpawn>& ActorsToSpawn);
	virtual void DestroyEquipmentActors();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//Set weapon owner pawn
	void SetInstigator(const TObjectPtr<APawn>& Instigator);
	void SetItemInstance(const TObjectPtr<UGZInventoryItemInstance>& ItemInstance);
	void SetEquipmentDefClass(const TSubclassOf<UGZEquipmentDefinition>& EquipmentDefClass);
	TArray<FGameplayAbilitySpecHandle>& GetGrantedAbilitySpecHandle();

	//Get weapon owner pawn
	UFUNCTION(BlueprintPure, Category=WeaponInstance)
	APawn* GetInstigator() const { return Instigator; }

	UFUNCTION(BlueprintPure, Category=WeaponInstance)
	UGZInventoryItemInstance* GetItemInstance() const { return ItemInstance; }

	UFUNCTION(BlueprintPure, Category=WeaponInstance)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	UFUNCTION(BlueprintPure, Category=WeaponInstance)
	TSubclassOf<UGZEquipmentDefinition> GetEquipmentDefClass() const { return EquipmentDefClass; }

	UFUNCTION(BlueprintPure, Category=WeaponInstance)
	UGZEquipmentDefinition* GetEquipmentDefinition() const { return EquipmentDefinition; }

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnEquippedFX();
	UFUNCTION(BlueprintImplementableEvent)
	void OnUnequippedFX();

	UPROPERTY(VisibleAnywhere, Replicated, Category="Equipment")
	TObjectPtr<APawn> Instigator; //weapon owner pawn
	UPROPERTY(VisibleAnywhere, Replicated, Category="Equipment")
	TObjectPtr<UGZInventoryItemInstance> ItemInstance;
	UPROPERTY(VisibleAnywhere, Replicated, Category="Equipment")
	TSubclassOf<UGZEquipmentDefinition> EquipmentDefClass;
	UPROPERTY(VisibleAnywhere, Replicated, Category="Equipment")
	TObjectPtr<UGZEquipmentDefinition> EquipmentDefinition;
	UPROPERTY(VisibleAnywhere, Replicated, Category="Equipment")
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandle;
	UPROPERTY(VisibleAnywhere, Replicated, Category="Equipment")
	TArray<TObjectPtr<AActor>> SpawnedActors;

private:
};
