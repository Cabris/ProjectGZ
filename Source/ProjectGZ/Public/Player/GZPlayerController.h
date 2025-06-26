// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GZPlayerController.generated.h"

/**
 * 
 */
class AGZCharacterBase;
class UInputMappingContext;;
class UInputAction;
struct FInputActionValue;

UCLASS()
class PROJECTGZ_API AGZPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AGZPlayerController();
	virtual  void SetupInputComponent() override;
	virtual  void BeginPlay() override;
	virtual  void PlayerTick(float DeltaTime) override;
protected:
private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> GZContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AimAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> StrafeAction;
	void Move(const FInputActionValue& inputActionValue);
	void Jump(const FInputActionValue& inputActionValue);
	void Crouch(const FInputActionValue& inputActionValue);
	void Look(const FInputActionValue& inputActionValue);
	void Aim(const FInputActionValue& inputActionValue);
	void Strafe(const FInputActionValue& inputActionValue);
	AGZCharacterBase* GetGZCharacter() const;
};
