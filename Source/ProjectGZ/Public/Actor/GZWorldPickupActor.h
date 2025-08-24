#pragma once

#include "CoreMinimal.h"
#include "GZInteractableActor.h"
#include "GameFramework/Actor.h"
#include "Interfactions/GZCollectable.h"
#include "Interfactions/GZInteractable.h"
#include "GZWorldPickupActor.generated.h"


class UGZInventoryItemDefinition;

UCLASS()
class PROJECTGZ_API AGZWorldPickupActor : public AGZInteractableActor, public IGZCollectable
{
	GENERATED_BODY()

public:
	AGZWorldPickupActor();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	//IGZCollectable
	virtual const TSubclassOf<UGZInventoryItemDefinition>& GetItemDefinitionClass() const override;
	virtual void ConsumeItemQuantity(int32 Quantity) override;
	virtual void OnCollected_Implementation() override;
	//IGZCollectable

protected:
	virtual void SetupComponents() override;

	UPROPERTY(EditDefaultsOnly, Category="Default|Item")
	TSubclassOf<UGZInventoryItemDefinition> ItemDefinitionClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default|Item")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category="Default|Item")
	TObjectPtr<UStaticMesh> DefaultMesh;
};
