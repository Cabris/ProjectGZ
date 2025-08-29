#include "Equipment/GZEquipmentInstance.h"
#include "Equipment/GZEquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

void UGZEquipmentInstance::OnEquipped()
{
	OnEquippedFX();
}

void UGZEquipmentInstance::OnUnequipped()
{
	OnUnequippedFX();
}

void UGZEquipmentInstance::SpawnEquipmentActors(TArray<FGZEquipmentActorToSpawn>& ActorsToSpawn)
{
	auto Owner = GetOuter();
	if (!Owner)return;
	auto OwnerPawn = Cast<APawn>(Owner);
	if (!OwnerPawn)return;
	USceneComponent* AttachTarget = OwnerPawn->GetRootComponent();
	auto Character = Cast<ACharacter>(Owner);
	if (Character && Character->GetMesh())
	{
		AttachTarget = Character->GetMesh();
	}
	for (auto& ActorDef : ActorsToSpawn)
	{
		AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(ActorDef.ActorToSpawn, FTransform::Identity, OwnerPawn);
		if (NewActor)
		{
			NewActor->SetReplicates(true);
			NewActor->FinishSpawning(FTransform::Identity, true);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, ActorDef.AttachSocket);
			NewActor->SetActorRelativeTransform(ActorDef.AttachTransform);
			SpawnedActors.Add(NewActor);
		}
	}
}

void UGZEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
	SpawnedActors.Empty();
}

void UGZEquipmentInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, SpawnedActors);
	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, ItemInstance);
	DOREPLIFETIME(ThisClass, EquipmentDefClass);
	DOREPLIFETIME(ThisClass, EquipmentDefinition);
	DOREPLIFETIME(ThisClass, GrantedAbilitySpecHandle)
}

void UGZEquipmentInstance::SetInstigator(const TObjectPtr<APawn>& InInstigator)
{
	this->Instigator = InInstigator;
}

void UGZEquipmentInstance::SetItemInstance(const TObjectPtr<UGZInventoryItemInstance>& InItemInstance)
{
	ItemInstance = InItemInstance;
}

TArray<FGameplayAbilitySpecHandle>& UGZEquipmentInstance::GetGrantedAbilitySpecHandle()
{
	return GrantedAbilitySpecHandle;
}

void UGZEquipmentInstance::SetEquipmentDefClass(const TSubclassOf<UGZEquipmentDefinition>& InEquipmentDefClass)
{
	EquipmentDefClass = InEquipmentDefClass;
	if (IsValid(EquipmentDefClass))
	{
		EquipmentDefinition = EquipmentDefClass.GetDefaultObject();
	}
}
