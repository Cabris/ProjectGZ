#pragma once
#include "CoreMinimal.h"
#include "AbilitySystem/GZInputGameplayAbilitySet.h"
#include "Components/SceneComponent.h"
#include "Data/GZAnimationLayerSet.h"
#include "Interfactions/GZPawnFeatureInterface.h"
#include "GZPawnFeatureComponent.generated.h"
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActorChangeEventSingnature, AActor*);

class UGZInventoryItemInstance;
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
	TObjectPtr<UGZAbilitySystemComponent> AbilitySystemComponent = nullptr;
	UPROPERTY()
	TObjectPtr<UGZAttributeSet> AttributeSet = nullptr;
	UPROPERTY()
	TObjectPtr<UGZInventoryManagerComponent> InventoryManager = nullptr;
	UPROPERTY()
	TObjectPtr<UGZEquipmentManagerComponent> EquipmentManager = nullptr;
	UPROPERTY()
	TObjectPtr<UGZWeaponMenuComponent> WeaponMenu = nullptr;
	UPROPERTY()
	TObjectPtr<APawn> Pawn = nullptr;
	UPROPERTY()
	TObjectPtr<APlayerState> PlayerState = nullptr;
};

//Owned by PlayerState
UCLASS()
class PROJECTGZ_API UGZPawnFeatureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZPawnFeatureComponent(); //initialize InventoryManager/EquipmentManager here
	static UGZPawnFeatureComponent* Get(AActor* Target);
	void SetupPawnFeature(const FPawnFeatureStruct& FeatureStruct);

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

	const TObjectPtr<APawn>& GetPawn()
	{
		return PawnFeatureStruct.Pawn;
	}

	const TObjectPtr<APlayerState>& GetPlayerState()
	{
		return PawnFeatureStruct.PlayerState;
	}

	void SetControlledPawn(APawn* NewPawn)
	{
		PawnFeatureStruct.Pawn = NewPawn;
	}

	void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);
	void OnInitializePawnFeature();
	virtual bool TryGrantItemToPawn(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefinitionClass, APawn* ReceivingPawn);
	virtual bool TryGrantEquipmentToPawn(UGZInventoryItemInstance* ItemInstance);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnActorChangeEventSingnature OnFocusActor;
	FOnActorChangeEventSingnature OnUnfocusActor;

protected:
	UPROPERTY(Replicated)
	FPawnFeatureStruct PawnFeatureStruct;

	UPROPERTY(EditDefaultsOnly, Category = "PawnFeature")
	TObjectPtr<UGZInputGameplayAbilitySet> AbilitySet;

private:
	UFUNCTION()
	void OnWeaponSlotSelected(UGZInventoryItemInstance* Instance, int SlotIdx);
};
