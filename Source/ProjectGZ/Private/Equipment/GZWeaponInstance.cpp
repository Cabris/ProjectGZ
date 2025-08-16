#include "Equipment/GZWeaponInstance.h"
#include "Equipment/GZEquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"


void UGZWeaponInstance::OnEquipped()
{
	OnEquippedFX();
}

void UGZWeaponInstance::OnUnequipped()
{
	OnUnequippedFX();
}

void UGZWeaponInstance::SpawnEquipmentActors(TArray<FGZEquipmentActorToSpawn>& ActorsToSpawn)
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
	for (auto ActorDef : ActorsToSpawn)
	{
		AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(ActorDef.ActorToSpawn, FTransform::Identity, OwnerPawn);
		if (NewActor)
		{
			NewActor->SetReplicates(true);
			NewActor->FinishSpawning(FTransform::Identity, true);
			NewActor->SetActorRelativeTransform(ActorDef.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, ActorDef.AttachSocket);
			SpawnedActors.Add(NewActor);
		}
	}
}

void UGZWeaponInstance::DestroyEquipmentActors()
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

void UGZWeaponInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, SpawnedActors);
	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, GrantedAbilitySpecHandle)
}

void UGZWeaponInstance::SetInstigator(const TObjectPtr<APawn>& InInstigator)
{
	this->Instigator = InInstigator;
}

TArray<FGameplayAbilitySpecHandle>& UGZWeaponInstance::GetGrantedAbilitySpecHandle()
{
	return GrantedAbilitySpecHandle;
}
