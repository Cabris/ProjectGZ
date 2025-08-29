#pragma once
#include "GZEquipmentInstance.h"
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
// Basic Definition for visual attribute such as model actors
UCLASS(Blueprintable, BlueprintType)
class PROJECTGZ_API UGZEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	FGameplayTag EquipmentAnimLayerTag;
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TSubclassOf<UGZEquipmentInstance> InstanceClass;
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TArray<FGZEquipmentActorToSpawn> ActorsToSpawn;
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	TArray<FInputAbilityEntry> GrantedAbilities;//Fire,Reload...
};
