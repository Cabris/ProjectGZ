#pragma once
#include "CoreMinimal.h"
#include "GZTagStackList.h"
#include "UObject/Object.h"
#include "GZWeaponInstance.generated.h"

struct FGameplayAbilitySpecHandle;
class FLifetimeProperty;
struct FGZEquipmentActorToSpawn;

//base class for weapon instance that used by player
//data stored in a weapon instance should only be its static attributes, ex: damage, range, CD time
//a weapon instance should focus on its attacking useage
//data such as ammo that will change in gameplay should be stored in inventory instance
UCLASS()
class PROJECTGZ_API UGZWeaponInstance : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnEquipped();
	virtual void OnUnequipped();
	virtual void SpawnEquipmentActors(TArray<FGZEquipmentActorToSpawn>& ActorsToSpawn);
	virtual void DestroyEquipmentActors();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetInstigator(const TObjectPtr<APawn>& Instigator);
	TArray<FGameplayAbilitySpecHandle>& GetGrantedAbilitySpecHandle();
protected:
	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActors;
	UPROPERTY(VisibleAnywhere, Replicated)
	TObjectPtr<APawn> Instigator; //who has this weapon
	UPROPERTY(Replicated)
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandle;

	UFUNCTION(BlueprintImplementableEvent)
	void OnEquippedFX();
	UFUNCTION(BlueprintImplementableEvent)
	void OnUnequippedFX();

private:
};
