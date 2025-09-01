// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GZCharacterBase.h"
#include "Interfactions/AimControllable.h"
#include "Interfactions/CameraControllable.h"
#include "Interfactions/GZCombatInterface.h"
#include "Interfactions/GZPawnFeatureInterface.h"
#include "Interfactions/Strafingable.h"
#include "GZPlayerCharacter.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimLayerChangedEventSingnature, TSubclassOf<UAnimInstance>, AnimLayer);

class UGZAnimationLayerSet;
class UCameraComponent;
class USpringArmComponent;
class UGZAimMotionComponent;

UCLASS()
class PROJECTGZ_API AGZPlayerCharacter : public AGZCharacterBase, public ICameraControllable, public IAimControllable,
                                         public IStrafingable, public IGZPawnFeatureInterface, public IGZCombatInterface
{
	GENERATED_BODY()

public:
	AGZPlayerCharacter();
	virtual void BeginPlay() override;
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
	virtual void OnEquipmentTagChanged_Implementation(FGameplayTag EquipmentTag) override;

	FORCEINLINE virtual UGZPawnFeatureComponent* GetPawnFeature() override
	{
		return UGZPawnFeature;
	}

	UFUNCTION(BlueprintImplementableEvent)
	void BindAnimLayerChangedEvent();

	UPROPERTY(BlueprintAssignable)
	FOnAnimLayerChangedEventSingnature OnAnimLayerChanged;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UGZAimMotionComponent> AimMotionComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UGZPawnFeatureComponent> UGZPawnFeature;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UGZAnimationLayerSet> AnimLayerSet;

private:
	bool bIsStrafing;
	void InitializePawnFeature();
};
