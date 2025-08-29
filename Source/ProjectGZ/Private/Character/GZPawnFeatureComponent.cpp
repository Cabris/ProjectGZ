#include "Character/GZPawnFeatureComponent.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "Equipment/GZEquipmentDefinition.h"
#include "Equipment/GZEquipmentManagerComponent.h"
#include "Equipment/GZWeaponMenuComponent.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Equipment/GZEquipmentInstance.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/Pawn.h"
#include "Interfactions/GZCombatInterface.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGZ/ProjectGZ.h"

UGZPawnFeatureComponent::UGZPawnFeatureComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UGZPawnFeatureComponent::SetupPawnFeature(const FPawnFeatureStruct& FeatureStruct)
{
	PawnFeatureStruct = FeatureStruct;
	PawnFeatureStruct.WeaponMenu->OnSlotSelected.AddDynamic(this, &ThisClass::OnWeaponSlotSelected);
}

void UGZPawnFeatureComponent::OnWeaponSlotSelected(UGZInventoryItemInstance* Instance, int SlotIdx)
{
	if (!IsValid(Instance))return;
	if (!Instance->IsEquipmentItem())return;
	auto EquipmentDefClass = Instance->GetItemDefinition()->GetEquipmentDef();
	UGZEquipmentDefinition* EquipmentDef = EquipmentDefClass.GetDefaultObject();
	if (IsValid(EquipmentDef))
	{
		if (GetPawn()->Implements<UGZCombatInterface>())
		{
			IGZCombatInterface::Execute_OnEquipmentTagChanged(GetPawn(), EquipmentDef->EquipmentAnimLayerTag);
		}
		else
		{
			Debug::Print(TEXT("OnWeaponSlotSelected: Pawn not Implements UGZCombatInterface"));
		}
	}
}


void UGZPawnFeatureComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	GetAbilitySystem()->InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	if (IsValid(AbilitySet))
	{
		TArray<FGameplayAbilitySpecHandle> Handles;
		GetAbilitySystem()->ApplyInputAbilitySet(AbilitySet, InOwnerActor, Handles);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AbilitySetClass not set"));
	}
}

bool UGZPawnFeatureComponent::TryGrantItemToPawn(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefinitionClass,
                                                 APawn* ReceivingPawn)
{
	if (!GetInventoryManager()) return false;
	UGZInventoryItemInstance* ItemInstance = GetInventoryManager()->AddItemDefToInventory(ItemDefinitionClass);
	if (!ItemInstance) return false;

	if (ItemInstance->IsEquipmentItem())
	{
		//try to equip item
		TryGrantEquipmentToPawn(ItemInstance);
	}
	return true;
}

bool UGZPawnFeatureComponent::TryGrantEquipmentToPawn(UGZInventoryItemInstance* ItemInstance)
{
	if (!IsValid(ItemInstance))
		return false;
	TSubclassOf<UGZEquipmentDefinition> EquipmentDefClass = ItemInstance->GetItemDefinition()->GetEquipmentDef();
	if (!IsValid(EquipmentDefClass))
		return false;
	//can be equipped
	UGZEquipmentDefinition* EquipmentDef = EquipmentDefClass.GetDefaultObject();
	if (!GetEquipmentManager() || !EquipmentDef || !IsValid(EquipmentDef->InstanceClass))
	{
		Debug::Print(TEXT("Failed to TryGrantEquipmentToPawn: EquipmentDef is NULL"));
		return false;
	}

	UGZEquipmentInstance* OwningWeaponInstance = GetEquipmentManager()->GetEquipmentInstanceByClass(
		EquipmentDef->InstanceClass);
	if (!OwningWeaponInstance)
	{
		//add weapon to Equipment
		UGZEquipmentInstance* NewWeaponInstance = GetEquipmentManager()->EquipItem(ItemInstance);
		if (!IsValid(NewWeaponInstance))
		{
			Debug::Print(TEXT("Failed to TryGrantEquipmentToPawn: NewWeaponInstance is NULL"));
			return false;
		}
		const auto& WeaponMenu = GetWeaponMenu();
		if (WeaponMenu)
		{
			WeaponMenu->AddWeaponToSlot(ItemInstance, 0);
			WeaponMenu->SetActiveWeaponSlot(0);
		}
	}
	else
	{
		Debug::Print(TEXT("TODO: add ammo to Weapon Instance"));
		//TODO: add ammo to Weapon Instance
		//ItemInstance->SetStackByTag("Ammo",100);
	}
	return true;
}

void UGZPawnFeatureComponent::OnInitializePawnFeature()
{
	GetAbilitySystem()->OnAbilityActorInfoSet();
}

void UGZPawnFeatureComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, PawnFeatureStruct);
}


UGZPawnFeatureComponent* UGZPawnFeatureComponent::Get(AActor* Target)
{
	if (!IsValid(Target)) return nullptr;
	IGZPawnFeatureInterface* PawnInterface = Cast<IGZPawnFeatureInterface>(Target);
	if (!PawnInterface) return nullptr;
	return PawnInterface->GetPawnFeature();
}
