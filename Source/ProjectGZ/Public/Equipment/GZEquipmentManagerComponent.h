// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GZEquipmentManagerComponent.generated.h"

class UGZEquipmentManagerComponent;
class UGZWeaponInstance;
class UGZEquipmentDefinition;
class UActorChannel;
class FOutBunch;
class FLifetimeProperty;
struct FReplicationFlags;

USTRUCT(BlueprintType)
struct FGZCarriedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TSubclassOf<UGZEquipmentDefinition> EquipmentDefinitionClass;
	UPROPERTY()
	TObjectPtr<UGZWeaponInstance> EquipmentInstance;
};

USTRUCT(BlueprintType)
struct FGZCarriedEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FGZCarriedEquipmentList()
		: Owner(nullptr)
	{
	}

	FGZCarriedEquipmentList(UObject* Owner)
		: Owner(Owner)
	{
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FGZCarriedEquipmentEntry, FGZCarriedEquipmentList>(
			Items, DeltaParms, *this);
	}

	UGZWeaponInstance* AddEntry(const TSubclassOf<UGZEquipmentDefinition>& DefinitionClass);
	void RemoveEntry(UGZWeaponInstance* EntryInstance);
	void RemoveAllEntries();
	const FGZCarriedEquipmentEntry* GetEntryByInstance(const TObjectPtr<UGZWeaponInstance>& Instance);
	const FGZCarriedEquipmentEntry* GetEntryByEquipmentDefClass(const TSubclassOf<UGZEquipmentDefinition>& DefinitionClass);

private:
	friend UGZEquipmentManagerComponent;
	UPROPERTY()
	TArray<FGZCarriedEquipmentEntry> Items;

	UPROPERTY()
	TObjectPtr<UObject> Owner = nullptr;
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
class PROJECTGZ_API UGZEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGZEquipmentManagerComponent();
	UGZWeaponInstance* GetEquipmentInstanceByClass(const TSubclassOf<UGZWeaponInstance>& InstanceClass);
	UGZWeaponInstance* EquipItem(const TSubclassOf<UGZEquipmentDefinition>& DefinitionClass);
	void UnEquipItem(UGZWeaponInstance* EquipmentInstance);
	virtual void ReadyForReplication() override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void UninitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated)
	FGZCarriedEquipmentList EquipmentList;
};
