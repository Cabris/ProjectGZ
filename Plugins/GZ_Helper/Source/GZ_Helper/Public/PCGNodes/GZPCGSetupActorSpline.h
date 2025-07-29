// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PCGSettings.h"

#include "GZPCGSetupActorSpline.generated.h"

UCLASS(MinimalAPI, BlueprintType)
class UGZPCGSetupActorSplineSettings : public UPCGSettings
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual FName GetDefaultNodeName() const override;
	virtual FText GetDefaultNodeTitle() const override;
	virtual FText GetNodeTooltipText() const override;
	virtual EPCGSettingsType GetType() const override { return EPCGSettingsType::Spatial; }
#endif
	UPROPERTY(meta = (PCG_Overridable))
	TSoftObjectPtr<AActor> TargetActor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spline", meta = (PCG_Overridable))
	bool bClosedLoop = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spline", meta = (PCG_Overridable))
	bool bLinear = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spline", meta = (PCG_Overridable))
	bool bGenerateSplineComponentInTargetActor = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spline", meta = (PCG_Overridable))
	FName AddActorTag = NAME_None;

protected:
	virtual TArray<FPCGPinProperties> InputPinProperties() const override;
	virtual TArray<FPCGPinProperties> OutputPinProperties() const override;
	virtual FPCGElementPtr CreateElement() const override;
};

class FGZPCGSetupActorSplineElement : public IPCGElement
{
protected:
	virtual bool ExecuteInternal(FPCGContext* Context) const override;
};
