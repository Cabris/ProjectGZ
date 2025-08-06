// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfactions/GZAbilitySystemInterface.h"
#include "GZCharacterBase.generated.h"

class UGZAbilitySystemComponent;

UCLASS()
class PROJECTGZ_API AGZCharacterBase : public ACharacter, public IGZAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGZCharacterBase();
	virtual UGZAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UGZAttributeSet* GetAttributeSet() const override;

protected:
private:
};
