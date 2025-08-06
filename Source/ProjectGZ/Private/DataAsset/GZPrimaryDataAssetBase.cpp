// Fill out your copyright notice in the Description page of Project Settings.
#include  "DataAsset/GZPrimaryDataAssetBase.h"

FPrimaryAssetId UGZPrimaryDataAssetBase::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(Type, GetFName());
}
