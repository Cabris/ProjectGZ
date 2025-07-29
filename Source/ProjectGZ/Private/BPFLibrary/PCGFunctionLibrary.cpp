// Fill out your copyright notice in the Description page of Project Settings.


#include "BPFLibrary/PCGFunctionLibrary.h"

AActor* UPCGFunctionLibrary::GetActorFromSotfPath(const FSoftObjectPath& SoftObjectPath)
{
	TSoftObjectPtr<AActor> ref = TSoftObjectPtr<AActor>(SoftObjectPath);
	UE_LOG(LogTemp, Warning, TEXT("UPCGFunctionLibrary:ref.IsValid %d" ), ref.IsValid());
	if (ref.IsValid())
	{
		auto actor = ref.LoadSynchronous();
		if (!actor)
		{
			UE_LOG(LogTemp, Warning, TEXT("UPCGFunctionLibrary:ref.LoadSynchronous Fail"));
		}
		return actor;
	}

	return  nullptr;
}
