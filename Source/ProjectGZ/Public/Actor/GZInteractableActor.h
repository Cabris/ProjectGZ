#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfactions/GZInteractable.h"
#include "GZInteractableActor.generated.h"

class USphereComponent;
UCLASS()
class PROJECTGZ_API AGZInteractableActor : public AActor, public IGZInteractable
{
	GENERATED_BODY()

public:
	AGZInteractableActor();
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
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default|Interaction")
	TObjectPtr<USphereComponent> InteractCollision;
	UPROPERTY(EditDefaultsOnly, Category="Default|Interaction")
	bool bIsInteractable = true;
	UPROPERTY(EditDefaultsOnly, Category="Default|Interaction")
	FText InteractionText;
	UPROPERTY(EditDefaultsOnly, Category="Default|Interaction")
	FGameplayTag InteractAbilityTriggerTag;
	
	virtual void BeginPlay() override;
	virtual void SetupComponents();
};
