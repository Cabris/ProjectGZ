// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GZPawnFeatureInterface.generated.h"

class UGZPawnFeatureComponent;
class UGZInventoryItemDefinition;

UINTERFACE()
class UGZPawnFeatureInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTGZ_API IGZPawnFeatureInterface
{
	GENERATED_BODY()
public:
	virtual UGZPawnFeatureComponent* GetPawnFeature() =0;
};
