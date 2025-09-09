#include "Equipment/GZEquipmentManagerComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GZPawnFeatureComponent.h"
#include "Engine/ActorChannel.h"
#include "Equipment/GZEquipmentDefinition.h"
#include "Equipment/GZEquipmentInstance.h"
#include "Interfactions/GZAbilitySystemInterface.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"


UGZEquipmentInstance* FGZCarriedEquipmentList::AddEntry(const FEquipmentListAddEntryParams& Params)
{
	if (!IsValid(Params.EquipmentOwner))return nullptr;
	UObject* EquipmentOwner = Params.EquipmentOwner.Get();

	FGZCarriedEquipmentEntry& EquipmentEntry = Items.AddDefaulted_GetRef();
	UGZEquipmentInstance* EquipmentInstance = NewObject<UGZEquipmentInstance>(EquipmentOwner, Params.EquipmentInstanceClass);
	EquipmentInstance->SetItemInstance(Params.ItemInstance);
	//EquipmentInstance->SetEquipmentDefClass(Params.EquipmentDefClass);

	if (IsValid(Params.EquipmentDefCDO))
	{
		auto& ActorsToSpawn = Params.EquipmentDefCDO->ActorsToSpawn;
		EquipmentInstance->SpawnEquipmentActors(ActorsToSpawn);
	}
	APawn* Instigator = Cast<APawn>(EquipmentOwner);
	if (IsValid(Instigator))
		EquipmentInstance->SetInstigator(Instigator);

	EquipmentEntry.EquipmentInstance = EquipmentInstance;
	EquipmentEntry.EquipmentDefinitionClass = Params.EquipmentDefClass;
	MarkItemDirty(EquipmentEntry);

	return EquipmentInstance;
}

void FGZCarriedEquipmentList::RemoveEntry(UGZEquipmentInstance* EquipmentInstance)
{
	if (!IsValid(EquipmentInstance))return;
	const int32 Idx = Items.IndexOfByPredicate([EquipmentInstance](const FGZCarriedEquipmentEntry& Entry)
	{
		return Entry.EquipmentInstance == EquipmentInstance;
	});
	EquipmentInstance->SetInstigator(nullptr);
	EquipmentInstance->SetItemInstance(nullptr);
	//EquipmentInstance->SetEquipmentDefClass(nullptr);
	EquipmentInstance->DestroyEquipmentActors();
	if (Idx != INDEX_NONE)
	{
		Items.RemoveAtSwap(Idx); //order is not important
		MarkArrayDirty();
	}
}

void FGZCarriedEquipmentList::RemoveAllEntries()
{
	if (Items.Num() > 0)
	{
		Items.Reset();
		MarkArrayDirty();
	}
}

const FGZCarriedEquipmentEntry* FGZCarriedEquipmentList::GetEntryByInstance(const TObjectPtr<UGZEquipmentInstance>& Instance)
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

UGZEquipmentManagerComponent::UGZEquipmentManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

UGZEquipmentInstance* UGZEquipmentManagerComponent::EquipItem(UGZInventoryItemInstance* ItemInstance)
{
	if (!IsValid(ItemInstance) || !IsValid(ItemInstance->GetItemDefinition()))
		return nullptr;

	UGZPawnFeatureComponent* PawnFeature = GetPawnFeature();
	if (!IsValid(PawnFeature))return nullptr;
	APawn* Pawn = PawnFeature->GetPlayerController()->GetPawn();

	if (!IsValid(Pawn))return nullptr;
	const FEquipmentListAddEntryParams Param(ItemInstance, Pawn);
	UGZEquipmentInstance* EquipmentInstance = EquipmentList.AddEntry(Param);
	if (!EquipmentInstance)return nullptr;

	TObjectPtr<UGZAbilitySystemComponent> ASC = PawnFeature->GetAbilitySystem();
	auto DefinitionClass = ItemInstance->GetItemDefinition()->GetEquipmentDef();
	if (!IsValid(DefinitionClass)) return nullptr;

	for (FInputAbilityEntry& GrantedAbility : DefinitionClass.GetDefaultObject()->GrantedAbilities)
	{
		ASC->GiveEquipmentGrantedAbility(GrantedAbility, EquipmentInstance, EquipmentInstance->GetGrantedAbilitySpecHandle());
	}

	EquipmentInstance->OnEquipped();
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(EquipmentInstance);
	}
	return EquipmentInstance;
}

bool UGZEquipmentManagerComponent::HasInstanceByClass(const TSubclassOf<UGZEquipmentInstance>& InstanceClass)
{
	return IsValid(GetInstanceByClass(InstanceClass));
}

void UGZEquipmentManagerComponent::UnEquip(UGZEquipmentInstance* EquipmentInstance)
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
			EquipmentInstance->GetGrantedAbilitySpecHandle().Reset();
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
		UGZEquipmentInstance* Instance = Entry.EquipmentInstance;
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
		UnEquip(Entry.EquipmentInstance);
	}
	EquipmentList.RemoveAllEntries();
}

void UGZEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, EquipmentList);
}

UGZEquipmentInstance* UGZEquipmentManagerComponent::GetInstanceByClass(const TSubclassOf<UGZEquipmentInstance>& InstanceClass)
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

UGZEquipmentInstance* UGZEquipmentManagerComponent::GetInstanceByItem(const UGZInventoryItemInstance* ItemInstance)
{
	for (FGZCarriedEquipmentEntry& Entry : EquipmentList.Items)
	{
		auto Instance = Entry.EquipmentInstance;
		if (IsValid(Instance) && Instance->GetItemInstance() == ItemInstance)
		{
			return Instance;
		}
	}
	return nullptr;
}
