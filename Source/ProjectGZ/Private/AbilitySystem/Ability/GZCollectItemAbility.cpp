#include "AbilitySystem/Ability/GZCollectItemAbility.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Character/GZCharacterBase.h"
#include "Character/GZPawnFeatureComponent.h"
#include "Interfactions/GZCollectable.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"


bool UGZCollectItemAbility::ActivateAbilityInternal(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData,
                                                    AActor* InteractableActor,
                                                    IGZInteractable* Interactable)
{
	if (!IsValid(InteractableActor)) return false;
	TargetActor = InteractableActor;
	if (CollectMontage) // Ensure you have a valid UAnimMontage asset assigned
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None, // Or a specific task name
			CollectMontage,
			1.0f // Play rate
			//,MontageSectionName // Optional: Specific section to play
		);

		// Bind delegates to handle montage completion, interruption, or blending out
		PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		//PlayMontageTask->OnBlendOut.AddDynamic(this, &UMyGameplayAbility::OnMontageBlendOut);
		PlayMontageTask->ReadyForActivation(); // Activates the task
		return true;
	}
	else
	{
		bool bIsSuccessed = CollectItem();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, !bIsSuccessed);
		return bIsSuccessed;
	}
}

UGZInventoryManagerComponent* UGZCollectItemAbility::GetInventoryManager() const
{
	return GetPawnFeature()->GetInventoryManager();
}

bool UGZCollectItemAbility::CollectItem()
{
	UGZInventoryManagerComponent* InventoryManager = GetInventoryManager();
	IGZCollectable* Collectable = Cast<IGZCollectable>(TargetActor);
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

	IGZCollectable::Execute_OnCollected(TargetActor);
	if (bConsumeItemQuantity)
	{
		Collectable->ConsumeItemQuantity(1);
	}
	//Do this in Item Actor may be better?
	/*if (HasAuthority(&ActivationInfo))
	{
		InteractableActor->Destroy();
	}*/
	return true;
}

void UGZCollectItemAbility::OnMontageCompleted()
{
	bool bIsSuccessed = CollectItem();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, !bIsSuccessed);
}

void UGZCollectItemAbility::OnMontageInterrupted()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}
