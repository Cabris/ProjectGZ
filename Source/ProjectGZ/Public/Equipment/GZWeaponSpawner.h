#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "GZWeaponSpawner.generated.h"

class UGZInventoryItemDefinition;
class UGZPickupDefinitionDataAsset;
class UGZEquipmentDefinition;
struct FTimerHandle;

UCLASS()
class PROJECTGZ_API AGZWeaponSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGZWeaponSpawner();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void AttemptPickUpWeapon(APawn* ReceivingPawn);
	bool GiveWeapon(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefinitionClass,  APawn* ReceivingPawn);
	void StartTimer();
	void ResetCoolDown();
	void OnCoolDownTimerComplete();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void PlaySpawnerReadyFx();
	// Called when the game starts or when spawned
	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Definition")
	TObjectPtr<UGZPickupDefinitionDataAsset> PickupItemDefinition;
	UPROPERTY(BlueprintReadOnly, Category = "Spawner")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "Spawner", Replicated)
	bool bCanPickup;
	FTimerHandle CoolDownTimerHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	float CoolDownTime;
};
