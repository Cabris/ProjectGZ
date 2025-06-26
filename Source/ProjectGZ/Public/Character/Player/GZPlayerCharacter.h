// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GZCharacterBase.h"
#include "Interfactions/AimControllable.h"
#include "Interfactions/CameraControllable.h"
#include "Interfactions/Strafingable.h"
#include "GZPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGZAimMotionComponent;

UCLASS()
class PROJECTGZ_API AGZPlayerCharacter : public AGZCharacterBase, public ICameraControllable, public IAimControllable,
                                         public IStrafingable
{
	GENERATED_BODY()

public:
	AGZPlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void PitchCamera(float AxisValue) override;
	virtual void YawCamera(float AxisValue) override;
	virtual UGZAimMotionComponent* GetAimMotionComponent() override;
	virtual void Strafe() override;
	virtual void Unstrafe() override;
	virtual bool IsStrafing() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(EditAnywhere, Category = "Aim")
	TObjectPtr<UGZAimMotionComponent> AimMotionComponent;

private:
	bool bIsStrafing;
};
