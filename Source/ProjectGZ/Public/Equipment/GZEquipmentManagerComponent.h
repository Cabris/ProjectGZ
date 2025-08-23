// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GZEquipmentDefinition.h"
#include "Character/GZPawnComponent.h"
#include "Components/ActorComponent.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GZEquipmentManagerComponent.generated.h"

class UGZInventoryItemInstance;
class UGZEquipmentManagerComponent;
class UGZEquipmentInstance;
class UGZEquipmentDefinition;
class UActorChannel;
class FOutBunch;
class FLifetimeProperty;
struct FReplicationFlags;

USTRUCT()
struct FEquipmentListAddEntryParams
{
	GENERATED_BODY()
	FEquipmentListAddEntryParams(){}
	FEquipmentListAddEntryParams(UGZInventoryItemInstance* InItemInstance, UObject* InEquipmentOwner)
	{
		if (!IsValid(InItemInstance))
			return;
		ItemInstance = InItemInstance;
		EquipmentOwner = InEquipmentOwner;

		if (UGZInventoryItemDefinition* ItemDefinition = InItemInstance->GetItemDefinition())
			EquipmentDefClass = ItemDefinition->GetEquipmentDef();
		if (!IsValid(EquipmentDefClass))
			return;

		EquipmentDefCDO = EquipmentDefClass.GetDefaultObject();
		if (EquipmentDefCDO)
		{
			EquipmentInstanceClass = EquipmentDefCDO->InstanceClass;
		}
	}

	UPROPERTY()
	TSubclassOf<UGZEquipmentDefinition> EquipmentDefClass;
	UPROPERTY()
	TSubclassOf<UGZEquipmentInstance> EquipmentInstanceClass;

	UPROPERTY()
	TObjectPtr<UObject> EquipmentOwner = nullptr;
	UPROPERTY()
	TObjectPtr<UGZInventoryItemInstance> ItemInstance = nullptr;
	UPROPERTY()
	TObjectPtr<UGZEquipmentDefinition> EquipmentDefCDO = nullptr;
};

USTRUCT(BlueprintType)
struct FGZCarriedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TSubclassOf<UGZEquipmentDefinition> EquipmentDefinitionClass;
	UPROPERTY()
	TObjectPtr<UGZEquipmentInstance> EquipmentInstance;
};

USTRUCT(BlueprintType)
struct FGZCarriedEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FGZCarriedEquipmentList()
	{
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FGZCarriedEquipmentEntry, FGZCarriedEquipmentList>(
			Items, DeltaParms, *this);
	}

	UGZEquipmentInstance* AddEntry(const FEquipmentListAddEntryParams& Params);
	void RemoveEntry(UGZEquipmentInstance* EntryInstance);
	void RemoveAllEntries();
	const FGZCarriedEquipmentEntry* GetEntryByInstance(const TObjectPtr<UGZEquipmentInstance>& Instance);
	const FGZCarriedEquipmentEntry* GetEntryByEquipmentDefClass(const TSubclassOf<UGZEquipmentDefinition>& DefinitionClass);

private:
	friend UGZEquipmentManagerComponent;
	UPROPERTY()
	TArray<FGZCarriedEquipmentEntry> Items;
};

//this is required for NetDeltaSerialize can be call
template <>
struct TStructOpsTypeTraits<FGZCarriedEquipmentList> : public TStructOpsTypeTraitsBase2<FGZCarriedEquipmentList>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTGZ_API UGZEquipmentManagerComponent : public UGZPawnComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGZEquipmentManagerComponent();
	UGZEquipmentInstance* GetEquipmentInstanceByClass(const TSubclassOf<UGZEquipmentInstance>& InstanceClass);
	UGZEquipmentInstance* EquipItem(UGZInventoryItemInstance* ItemInstance);
	void UnEquipItem(UGZEquipmentInstance* EquipmentInstance);
	virtual void ReadyForReplication() override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void UninitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated)
	FGZCarriedEquipmentList EquipmentList;
};
