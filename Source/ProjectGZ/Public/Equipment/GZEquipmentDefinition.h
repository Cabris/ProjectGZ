#pragma once
#include "GZWeaponInstance.h"
#include "AbilitySystem/GZInputGameplayAbilitySet.h"
#include "GameFramework/Actor.h"
#include "GZEquipmentDefinition.generated.h"

USTRUCT(BlueprintType)
struct FGZEquipmentActorToSpawn
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category="Equipment|Actor")
	TSubclassOf<AActor> ActorToSpawn;
	UPROPERTY(EditDefaultsOnly, Category="Equipment|Actor")
	FName MuzzleSocket;
	UPROPERTY(EditDefaultsOnly, Category="Equipment|Actor")
	FName AttachSocket;
	UPROPERTY(EditDefaultsOnly, Category="Equipment|Actor")
	FTransform AttachTransform;
};

// Definition of equipment that can be applied to player
UCLASS(Blueprintable, BlueprintType)
class PROJECTGZ_API UGZEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TSubclassOf<UGZWeaponInstance> InstanceClass;
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TArray<FGZEquipmentActorToSpawn> ActorsToSpawn;
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TArray<FInputAbilityEntry> GrantedAbilities;//Fire,Reload...
};
