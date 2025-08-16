// Fill out your copyright notice in the Description page of Project Settings.
#include  "ProjectGZ/Public/Data/GZPrimaryDataAssetBase.h"

FPrimaryAssetId UGZPrimaryDataAssetBase::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(Type, GetFName());
}
