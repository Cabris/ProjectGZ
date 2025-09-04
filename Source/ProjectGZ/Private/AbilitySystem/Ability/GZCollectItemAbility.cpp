#include "AbilitySystem/Ability/GZCollectItemAbility.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Character/GZCharacterBase.h"
#include "Character/GZPawnFeatureComponent.h"
#include "Interfactions/GZCollectable.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"


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
	IGZCollectable* Collectable = Cast<IGZCollectable>(TargetActor);
	bool bIsCollect = CollectItemToInventory(Collectable);
	if (!bIsCollect)
	{
		PlayClientCollectItemFailFX();
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		UE_LOG(LogTemp, Warning, TEXT("[UGZCollectItemAbility::CollectItemTask] CollectItemToInventory Fail, IsLocalControlled=%d"),
		       IsLocalControlled());
		return false;
	}
	PlayClientCollectItemFX(TargetActor);
	IGZCollectable::Execute_OnCollected(TargetActor);
	CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	UE_LOG(LogTemp, Warning, TEXT("[UGZCollectItemAbility::CollectItemTask] CollectItemToInventory Success, IsLocalControlled=%d"),
	       IsLocalControlled());
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

	if (!IsLocalControlled()) //On Server
	{
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
	}
	UE_LOG(LogTemp, Warning, TEXT("UGZCollectItemAbility::CollectItemToInventory: Success"));
	return true;
}

UGZInventoryManagerComponent* UGZCollectItemAbility::GetInventoryManager() const
{
	return GetPawnFeature()->GetInventoryManager();
}
