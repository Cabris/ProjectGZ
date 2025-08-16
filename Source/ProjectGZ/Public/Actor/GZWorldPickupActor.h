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
	virtual FVector GetWorldPosition() const override;
	virtual FText GetInteravtionText() const override;
	virtual bool IsInteractable() const override;
	virtual const TSubclassOf<UGZInteractAbility>& GetInteractAbilityClass() override;

	virtual void DoInteract_Implementation() override;
	virtual void OnBeginFocus_Implementation() override;
	virtual void OnEndFocus_Implementation() override;

	virtual const TSubclassOf<UGZInventoryItemDefinition>& GetItemDefinitionClass() const override;

protected:
	UPROPERTY()
	TObjectPtr<USphereComponent> InteractCollision;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category="Pickup")
	bool bIsInteractable = true;
	UPROPERTY(EditDefaultsOnly, Category="Pickup")
	FText InteravtionText;
	UPROPERTY(EditDefaultsOnly, Category="Pickup")
	TSubclassOf<UGZInventoryItemDefinition> ItemDefinitionClass;
	UPROPERTY(EditDefaultsOnly, Category="Pickup")
	TSubclassOf<UGZInteractAbility> InteractAbilityClass;
};
