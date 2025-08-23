#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfactions/GZCollectable.h"
#include "Interfactions/GZInteractable.h"
#include "GZWorldPickupActor.generated.h"

class USphereComponent;
class UGZInventoryItemDefinition;

UCLASS()
class PROJECTGZ_API AGZWorldPickupActor : public AActor, public IGZInteractable, public IGZCollectable
{
	GENERATED_BODY()

public:
	AGZWorldPickupActor();
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PostLoad() override;

	//IGZInteractable
	virtual FVector GetWorldPosition() const override;
	virtual FText GetInteractionText() const override;
	virtual bool IsInteractable() const override;
	virtual const FGameplayTag& GetInteractAbilityTriggerTag() override;
	virtual void DoInteract_Implementation() override;
	virtual void OnBeginFocus_Implementation() override;
	virtual void OnEndFocus_Implementation() override;
	//IGZCollectable

	//IGZCollectable
	virtual const TSubclassOf<UGZInventoryItemDefinition>& GetItemDefinitionClass() const override;
	virtual void ConsumeItemQuantity(int32 Quantity) override;
	virtual void OnCollected_Implementation() override;
	//IGZCollectable

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default|Pickup")
	TObjectPtr<USphereComponent> InteractCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default|Pickup")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category="Default|Pickup")
	bool bIsInteractable = true;
	UPROPERTY(EditDefaultsOnly, Category="Default|Pickup")
	FText InteractionText;
	UPROPERTY(EditDefaultsOnly, Category="Default|Pickup")
	TSubclassOf<UGZInventoryItemDefinition> ItemDefinitionClass;
	UPROPERTY(EditDefaultsOnly, Category="Default|Pickup")
	FGameplayTag InteractAbilityTriggerTag;
	UPROPERTY(EditDefaultsOnly, Category="Default|Pickup")
	TObjectPtr<UStaticMesh> DefaultMesh;

private:
	void SetupComponents();
	bool bIsComponentsSetup = false;
};
