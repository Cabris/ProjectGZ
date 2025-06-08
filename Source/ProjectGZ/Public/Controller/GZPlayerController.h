// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GZPlayerController.generated.h"

/**
 * 
 */
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
	void Move(const FInputActionValue& inputActionValue);
};
