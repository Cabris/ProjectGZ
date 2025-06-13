// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GZCharacterBase.h"
#include "Interfactions/CameraControllable.h"
#include "GZPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
UCLASS()
class PROJECTGZ_API AGZPlayerCharacter : public AGZCharacterBase, public ICameraControllable
{
	GENERATED_BODY()
public:
	AGZPlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void PitchCamera(float AxisValue) override;
	virtual void YawCamera(float AxisValue) override;
protected:
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;
private:
};
