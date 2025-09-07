#include "AbilitySystem/Ability/GZCollectItemAbility.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Character/GZCharacterBase.h"
#include "Character/GZPawnFeatureComponent.h"
#include "Interfactions/GZCollectable.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "ProjectGZ/ProjectGZ.h"


bool UGZCollectItemAbility::ActivateAbilityInternal(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData,
                                                    AActor* InteractableActor,
                                                    IGZInteractable* Interactable)
{
	if (!IsValid(InteractableActor)) return false;
	TargetActor = InteractableActor;
	return CollectItemTask();
}

bool UGZCollectItemAbility::CollectItemTask()
{
	if (IsNetAuthority()) //On Server or Single player
	{
		IGZCollectable* Collectable = Cast<IGZCollectable>(TargetActor);
		if (!CollectItemToInventory(Collectable))
		{
			PlayCollectItemFailFX();
			UE_LOG(LogTemp, Warning, TEXT("[UGZCollectItemAbility::CollectItemTask] CollectItemToInventory Fail, IsLocalControlled=%s"),
			       *TargetActor->GetFullName());
			return false;
		}
	}
	PlayCollectItemFX(TargetActor);
	IGZCollectable::Execute_OnCollected(TargetActor);
	CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	return true;
}

bool UGZCollectItemAbility::CollectItemToInventory(IGZCollectable* Collectable) const
{
	if (!Collectable)
	{
		UE_LOG(LogTemp, Error, TEXT("UGZCollectItemAbility::CollectItemToInventory: Can not cast InteractableActor to IGZCollectable!"));
		return false;
	}

	UGZInventoryManagerComponent* InventoryManager = GetInventoryManager();
	if (!InventoryManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UGZCollectItemAbility::CollectItemToInventory: InventoryManager is null!"));
		return false;
	}

	UGZInventoryItemInstance* Instance = InventoryManager->AddItemDefToInventory(Collectable->GetItemDefinitionClass());
	if (!Instance)
	{
		UE_LOG(LogTemp, Error, TEXT("UGZCollectItemAbility::CollectItemToInventory: AddItemDefToInventory fail!"));
		return false;
	}
	UGZPawnFeatureComponent* PawnFeature = GetPawnFeature();
	if (IsValid(PawnFeature) && bTryEquipItem)
	{
		bool Success = PawnFeature->TryGrantEquipmentToPawn(Instance);
		UE_LOG(LogTemp, Warning, TEXT("UGZCollectItemAbility::CollectItemToInventory: TryGrantEquipmentToPawn: %d"), Success);
	}

	UE_LOG(LogTemp, Warning, TEXT("UGZCollectItemAbility::CollectItemToInventory: Success"));
	return true;
}

UGZInventoryManagerComponent* UGZCollectItemAbility::GetInventoryManager() const
{
	return GetPawnFeature()->GetInventoryManager();
}
