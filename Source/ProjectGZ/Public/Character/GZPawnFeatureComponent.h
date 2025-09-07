#pragma once
#include "CoreMinimal.h"
#include "AbilitySystem/GZInputGameplayAbilitySet.h"
#include "Components/SceneComponent.h"
#include "Data/GZAnimationLayerSet.h"
#include "Interfactions/GZPawnFeatureInterface.h"
#include "GZPawnFeatureComponent.generated.h"
class AGZPlayerState;
class AGZPlayerController;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActorChangeEventSingnature, AActor*);

class UGZInventoryItemInstance;
class UGZAttributeSet;
class UGZAbilitySystemComponent;
class UGZInventoryManagerComponent;
class UGZEquipmentManagerComponent;
class UGZInventoryItemDefinition;
class UGZWeaponSlotComponent;
class APawn;

USTRUCT()
struct FPawnFeatureStruct
{
	GENERATED_BODY()
	UPROPERTY()
	TObjectPtr<UGZAbilitySystemComponent> AbilitySystem = nullptr;
	UPROPERTY()
	TObjectPtr<UGZInventoryManagerComponent> InventoryManager = nullptr;
	UPROPERTY()
	TObjectPtr<UGZEquipmentManagerComponent> EquipmentManager = nullptr;
	UPROPERTY()
	TObjectPtr<UGZWeaponSlotComponent> WeaponMenu = nullptr;
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController = nullptr;
	UPROPERTY()
	TObjectPtr<APlayerState> PlayerState = nullptr;
	UPROPERTY()
	TObjectPtr<APawn> Pawn = nullptr;
	UPROPERTY()
	int HasAuthority = -1;
};

//Owned by PlayerState
UCLASS()
class PROJECTGZ_API UGZPawnFeatureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZPawnFeatureComponent();
	void InitializePawnFeature(AGZPlayerController* PC, AGZPlayerState* PS, APawn* Pawn);
	static UGZPawnFeatureComponent* Get(AActor* Target);

	APawn* GetPawn();
	UGZAbilitySystemComponent* GetAbilitySystem();
	UGZInventoryManagerComponent* GetInventoryManager();
	UGZEquipmentManagerComponent* GetEquipmentManager();
	UGZWeaponSlotComponent* GetWeaponSlot();
	APlayerController* GetPlayerController();

	virtual bool TryGrantItemToPawn(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefinitionClass, APawn* ReceivingPawn);
	virtual bool TryGrantEquipmentToPawn(UGZInventoryItemInstance* ItemInstance);

	FOnActorChangeEventSingnature OnFocusActor;
	FOnActorChangeEventSingnature OnUnfocusActor;

protected:
	FPawnFeatureStruct PawnFeatureStruct;
private:
	UFUNCTION()
	void OnWeaponSlotSelected(UGZInventoryItemInstance* Instance, int SlotIdx);
};
