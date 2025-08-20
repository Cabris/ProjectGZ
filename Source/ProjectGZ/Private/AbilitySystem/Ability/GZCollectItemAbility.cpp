#include "AbilitySystem/Ability/GZCollectItemAbility.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Character/GZCharacterBase.h"
#include "Character/GZPawnFeatureComponent.h"
#include "Interfactions/GZCollectable.h"

bool UGZCollectItemAbility::ActivateAbilityInternal(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData,
                                                    AActor* InteractableActor,
                                                    IGZInteractable* Interactable)
{
	UGZInventoryManagerComponent* InventoryManager = GetInventoryManager();
	IGZCollectable* Collectable = Cast<IGZCollectable>(InteractableActor);
	if (!Collectable)
	{
		UE_LOG(LogTemp, Error, TEXT("Can not cast InteractableActor to IGZCollectable!"));
		return false;
	}
	if (!InventoryManager)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryManager is null!"));
		return false;
	}
	auto Instance = InventoryManager->AddItemDefToInventory(Collectable->GetItemDefinitionClass());
	if (!Instance)
	{
		UE_LOG(LogTemp, Error, TEXT("AddItemDefToInventory fail!"));
		return false;
	}

	IGZCollectable::Execute_OnCollected(InteractableActor);
	if (bConsumeItemQuantity)
	{
	}
	/*if (HasAuthority(&ActivationInfo))
	{
		InteractableActor->Destroy();
	}*/
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	return true;
}

UGZInventoryManagerComponent* UGZCollectItemAbility::GetInventoryManager() const
{
	return GetPawnFeature()->GetInventoryManager();
}
