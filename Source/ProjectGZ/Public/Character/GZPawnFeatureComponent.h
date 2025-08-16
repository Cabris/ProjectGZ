#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Interfactions/GZPawnFeatureInterface.h"
#include "GZPawnFeatureComponent.generated.h"

class UGZAttributeSet;
class UGZAbilitySystemComponent;
class UGZInventoryManagerComponent;
class UGZEquipmentManagerComponent;
class UGZInventoryItemDefinition;
class UGZWeaponMenuComponent;
class APawn;

USTRUCT()
struct FPawnFeatureStruct
{
	GENERATED_BODY()
	UPROPERTY()
	TObjectPtr<UGZAbilitySystemComponent> AbilitySystemComponent=nullptr;
	UPROPERTY()
	TObjectPtr<UGZAttributeSet> AttributeSet=nullptr;
	UPROPERTY()
	TObjectPtr<UGZInventoryManagerComponent> InventoryManager=nullptr;
	UPROPERTY()
	TObjectPtr<UGZEquipmentManagerComponent> EquipmentManager=nullptr;
	UPROPERTY()
	TObjectPtr<UGZWeaponMenuComponent> WeaponMenu=nullptr;
};

//Owned by PlayerState
UCLASS()
class PROJECTGZ_API UGZPawnFeatureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZPawnFeatureComponent(); //initialize InventoryManager/EquipmentManager here

	const TObjectPtr<UGZAbilitySystemComponent>& GetAbilitySystem()
	{
		return PawnFeatureStruct.AbilitySystemComponent;
	}

	const TObjectPtr<UGZAttributeSet>& GetAttributeSet()
	{
		return PawnFeatureStruct.AttributeSet;
	}

	const TObjectPtr<UGZInventoryManagerComponent>& GetInventoryManager()
	{
		return PawnFeatureStruct.InventoryManager;
	}

	const TObjectPtr<UGZEquipmentManagerComponent>& GetEquipmentManager()
	{
		return PawnFeatureStruct.EquipmentManager;
	}

	const TObjectPtr<UGZWeaponMenuComponent>& GetWeaponMenu()
	{
		return PawnFeatureStruct.WeaponMenu;
	}

	void SetupPawnFeature(const FPawnFeatureStruct& FeatureStruct)
	{
		PawnFeatureStruct = FeatureStruct;
	}

	void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);
	virtual bool TryGrantItemToPawn(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefinitionClass, APawn* ReceivingPawn);
	void OnInitializePawnFeature();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated)
	FPawnFeatureStruct PawnFeatureStruct;
};
