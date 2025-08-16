#include "Equipment/GZEquipmentManagerComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/ActorChannel.h"
#include "Equipment/GZEquipmentDefinition.h"
#include "Equipment/GZWeaponInstance.h"
#include "Interfactions/GZAbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"


UGZWeaponInstance* FGZCarriedEquipmentList::AddEntry(const TSubclassOf<UGZEquipmentDefinition>& DefinitionClass)
{
	if (!IsValid(DefinitionClass))return nullptr;
	UGZEquipmentDefinition* CDO = DefinitionClass.GetDefaultObject();
	if (!CDO) return nullptr;
	auto InstanceType = CDO->InstanceClass;
	check(InstanceType);
	auto& ActorsToSpawn = CDO->ActorsToSpawn;
	auto Item = Items.AddDefaulted_GetRef();
	Item.EquipmentDefinitionClass = DefinitionClass;
	Item.EquipmentInstance = NewObject<UGZWeaponInstance>(Owner, InstanceType);
	Item.EquipmentInstance->SpawnEquipmentActors(ActorsToSpawn);
	return Item.EquipmentInstance;
}

void FGZCarriedEquipmentList::RemoveEntry(UGZWeaponInstance* EntryInstance)
{
	for (auto It = Items.CreateIterator(); It; ++It)
	{
		if (EntryInstance == It->EquipmentInstance)
		{
			EntryInstance->DestroyEquipmentActors();
			It.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FGZCarriedEquipmentList::RemoveAllEntries()
{
	bool HasChanged = false;
	for (auto It = Items.CreateIterator(); It; ++It)
	{
		if (It->EquipmentInstance)
			It->EquipmentInstance->DestroyEquipmentActors();
		It.RemoveCurrent();
		HasChanged = true;
	}
	if (HasChanged)
		MarkArrayDirty();
}

const FGZCarriedEquipmentEntry* FGZCarriedEquipmentList::GetEntryByInstance(const TObjectPtr<UGZWeaponInstance>& Instance)
{
	for (FGZCarriedEquipmentEntry& Entry : Items)
	{
		if (Entry.EquipmentInstance == Instance)
			return &Entry;
	}
	return nullptr;
}

const FGZCarriedEquipmentEntry* FGZCarriedEquipmentList::GetEntryByEquipmentDefClass(
	const TSubclassOf<UGZEquipmentDefinition>& DefinitionClass)
{
	for (FGZCarriedEquipmentEntry& Entry : Items)
	{
		if (Entry.EquipmentDefinitionClass == DefinitionClass)
			return &Entry;
	}
	return nullptr;
}

UGZEquipmentManagerComponent::UGZEquipmentManagerComponent(): EquipmentList(GetOwner())
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

UGZWeaponInstance* UGZEquipmentManagerComponent::EquipItem(const TSubclassOf<UGZEquipmentDefinition>& DefinitionClass)
{
	if (!IsValid(DefinitionClass)) return nullptr;
	UGZWeaponInstance* Instance = EquipmentList.AddEntry(DefinitionClass);
	if (!Instance)return nullptr;

	if (IGZAbilitySystemInterface* ASI = Cast<IGZAbilitySystemInterface>(GetOwner()))
	{
		if (UGZAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			for (FInputAbilityEntry& GrantedAbility : DefinitionClass.GetDefaultObject()->GrantedAbilities)
			{
				ASC->GiveEquipmentGrantedAbility(GrantedAbility, Instance, Instance->GetGrantedAbilitySpecHandle());
			}
		}
	}

	Instance->OnEquipped();
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Instance);
	}
	return Instance;
}

void UGZEquipmentManagerComponent::UnEquipItem(UGZWeaponInstance* EquipmentInstance)
{
	if (!EquipmentInstance)return;
	if (IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(EquipmentInstance);
	}

	if (IGZAbilitySystemInterface* ASI = Cast<IGZAbilitySystemInterface>(GetOwner()))
	{
		if (UGZAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			for (auto Handle : EquipmentInstance->GetGrantedAbilitySpecHandle())
				ASC->ClearAbility(Handle);
		}
	}

	EquipmentInstance->OnUnequipped();
	EquipmentList.RemoveEntry(EquipmentInstance);
}

void UGZEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	if (IsUsingRegisteredSubObjectList())
	{
		for (FGZCarriedEquipmentEntry& Entry : EquipmentList.Items)
		{
			auto Instance = Entry.EquipmentInstance;
			if (IsValid(Instance))
				AddReplicatedSubObject(Instance);
		}
	}
}

bool UGZEquipmentManagerComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool Ret = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (FGZCarriedEquipmentEntry& Entry : EquipmentList.Items)
	{
		UGZWeaponInstance* Instance = Entry.EquipmentInstance;
		if (IsValid(Instance))
		{
			Ret |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}
	return Ret;
}

void UGZEquipmentManagerComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
	for (FGZCarriedEquipmentEntry& Entry : EquipmentList.Items)
	{
		UnEquipItem(Entry.EquipmentInstance);
	}
	EquipmentList.RemoveAllEntries();
}

void UGZEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, EquipmentList);
}

UGZWeaponInstance* UGZEquipmentManagerComponent::GetEquipmentInstanceByClass(const TSubclassOf<UGZWeaponInstance>& InstanceClass)
{
	for (FGZCarriedEquipmentEntry& Entry : EquipmentList.Items)
	{
		auto Instance = Entry.EquipmentInstance;
		if (IsValid(Instance) && Instance->IsA(InstanceClass))
		{
			return Instance;
		}
	}
	return nullptr;
}
