// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GZCharacterBase.h"
#include "Interfactions/AimControllable.h"
#include "Interfactions/CameraControllable.h"
#include "Interfactions/GZPawnFeatureInterface.h"
#include "Interfactions/Strafingable.h"
#include "GZPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGZAimMotionComponent;

UCLASS()
class PROJECTGZ_API AGZPlayerCharacter : public AGZCharacterBase, public ICameraControllable, public IAimControllable,
                                         public IStrafingable, public IGZPawnFeatureInterface
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

	virtual UGZAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UGZAttributeSet* GetAttributeSet() const override;

	FORCEINLINE virtual UGZPawnFeatureComponent* GetPawnFeature() override
	{
		return UGZPawnFeature;
	}

protected:
	UPROPERTY(VisibleAnywhere, Category = "Character|Camera")
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, Category = "Character|Camera")
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Character|Aim")
	TObjectPtr<UGZAimMotionComponent> AimMotionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Character|AbilitySet")
	TObjectPtr<UGZInputGameplayAbilitySet> AbilitySet;

	UPROPERTY(VisibleAnywhere, Category = "Character|PawnFeature")
	TObjectPtr<UGZPawnFeatureComponent> UGZPawnFeature;

private:
	bool bIsStrafing;
	void InitializePawnFeature();
};
