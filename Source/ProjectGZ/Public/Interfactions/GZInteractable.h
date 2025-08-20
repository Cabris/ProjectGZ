#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "GZInteractable.generated.h"

class UGZInteractAbility;

UINTERFACE(BlueprintType, BlueprintType)
class UGZInteractable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTGZ_API IGZInteractable
{
	GENERATED_BODY()

public:
	//implemented in concrete actor c++ who implements this interface
	virtual FVector GetWorldPosition() const = 0;
	virtual FText GetInteractionText() const = 0;
	virtual bool IsInteractable() const = 0;
	virtual const FGameplayTag& GetInteractAbilityTriggerTag() =0;

	//implemented in concrete actor BP who implements this interface
	UFUNCTION(BlueprintNativeEvent, Category="Interactable")
	void DoInteract();
	UFUNCTION(BlueprintNativeEvent, Category="Interactable")
	void OnBeginFocus();
	UFUNCTION(BlueprintNativeEvent, Category="Interactable")
	void OnEndFocus();
};
