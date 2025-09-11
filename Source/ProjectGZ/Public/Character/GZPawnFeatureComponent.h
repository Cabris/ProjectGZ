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
class UGZWeaponSlotsComponent;
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
	TObjectPtr<UGZWeaponSlotsComponent> WeaponMenu = nullptr;
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
	void InitializePawnFeature(AGZPlayerController* PC, AGZPlayerState* PS, UGZAbilitySystemComponent* ASC, APawn* Pawn);
	static UGZPawnFeatureComponent* Get(AActor* Target);

	APawn* GetPawn();
	UGZAbilitySystemComponent* GetAbilitySystem();
	UGZInventoryManagerComponent* GetInventoryManager();
	UGZEquipmentManagerComponent* GetEquipmentManager();
	UGZWeaponSlotsComponent* GetWeaponSlotsManager();
	APlayerController* GetPlayerController();

	//add equipment to pawn
	virtual bool TryGrantEquipmentToPawn(UGZInventoryItemInstance* ItemInstance);
	//remove equipment to pawn
	virtual bool TryRemoveEquipmentFromPawn(UGZInventoryItemInstance* ItemInstance);
	FOnActorChangeEventSingnature OnFocusActor;
	FOnActorChangeEventSingnature OnUnfocusActor;

protected:
	FPawnFeatureStruct PawnFeatureStruct;

private:
	UFUNCTION()
	void OnActiveWeaponSlotChanged( int ActiveSlot);
	bool CheckComponents();
};
