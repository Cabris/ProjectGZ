// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/MyActor.h"


void FExampleItemEntry::PreReplicatedRemove(const struct FExampleArray& InArraySerializer)
{
	bool HasAuthority = InArraySerializer.MyActor->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("FExampleArray::FExampleItemEntry::PreReplicatedRemove: %d, HasAuthority: %d"),
	       InArraySerializer.Items.Num(),
	       HasAuthority);
}

void FExampleItemEntry::PostReplicatedAdd(const struct FExampleArray& InArraySerializer)
{
	bool HasAuthority = InArraySerializer.MyActor->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("FExampleArray::FExampleItemEntry::PostReplicatedAdd: %d, HasAuthority: %d"),
	       InArraySerializer.Items.Num(),
	       HasAuthority);
}

void FExampleItemEntry::PostReplicatedChange(const struct FExampleArray& InArraySerializer)
{
	bool HasAuthority = InArraySerializer.MyActor->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("FExampleArray::FExampleItemEntry::PostReplicatedChange: %d, HasAuthority: %d"),
	       InArraySerializer.Items.Num(),
	       HasAuthority);
}

void FExampleArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	bool HasAuthority = MyActor->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("FExampleArray::PostReplicatedAdd: %d, HasAuthority: %d"),
	       Items.Num(),
	       HasAuthority);
}

void FExampleArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	bool HasAuthority = MyActor->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("FExampleArray::PreReplicatedRemove: %d, HasAuthority: %d"),
	       Items.Num(),
	       HasAuthority);
}

void FExampleArray::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	bool HasAuthority = MyActor->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("FExampleArray::PostReplicatedChange: %d, HasAuthority: %d"),
	       Items.Num(),
	       HasAuthority);
}

// Sets default values
AMyActor::AMyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DeltaTest.MyActor = this;
}
