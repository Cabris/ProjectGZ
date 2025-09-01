// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "GZPlayerController.generated.h"

class UGZAbilitySystemComponent;
class AGZCharacterBase;
class UInputMappingContext;
class UGZDataAssetInputConfig;
class UInputAction;
struct FInputActionValue;

UCLASS()
class PROJECTGZ_API AGZPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGZPlayerController();
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;

protected:
private:
	UPROPERTY(EditAnywhere, Category = "Input")
	UGZDataAssetInputConfig* InputConfigDA;

	void Move(const FInputActionValue& inputActionValue);
	void Jump(const FInputActionValue& inputActionValue);
	void Crouch(const FInputActionValue& inputActionValue);
	void Look(const FInputActionValue& inputActionValue);
	void Aim(const FInputActionValue& inputActionValue);
	void Strafe(const FInputActionValue& inputActionValue);
	void Sprint(const FInputActionValue& inputActionValue);
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);
	AGZCharacterBase* GetGZCharacter() const;
	UGZAbilitySystemComponent* GetGZAbilitySystem() const;
};
