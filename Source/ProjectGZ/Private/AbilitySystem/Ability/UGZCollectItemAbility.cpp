#include "AbilitySystem/Ability/UGZCollectItemAbility.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Character/GZCharacterBase.h"
#include "Interfactions/GZCollectable.h"


bool UUGZCollectItemAbility::ActivateAbilityInternal(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                     const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData,
                                                     AActor* InteractableActor,
                                                     IGZInteractable* Interactable)
{
	auto InventoryManager = GetInventoryManager();
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
	if (HasAuthority(&ActivationInfo))
	{
		InteractableActor->Destroy();
	}
	return true;
}

UGZInventoryManagerComponent* UUGZCollectItemAbility::GetInventoryManager() const
{
	return GetCharacter()->GetComponentByClass<UGZInventoryManagerComponent>();
}
