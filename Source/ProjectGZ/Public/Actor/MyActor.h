// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "MyActor.generated.h"


class AMyActor;

USTRUCT()
struct FExampleItemEntry : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	// Your data:
	UPROPERTY()
	int32 ExampleIntProperty=0;

	UPROPERTY()
	float ExampleFloatProperty=0;


	/** 
	 * Optional functions you can implement for client side notification of changes to items; 
	 * Parameter type can match the type passed as the 2nd template parameter in associated call to FastArrayDeltaSerialize
	 * 
	 * NOTE: It is not safe to modify the contents of the array serializer within these functions, nor to rely on the contents of the array 
	 * being entirely up-to-date as these functions are called on items individually as they are updated, and so may be called in the middle of a mass update.
	 */
	void PreReplicatedRemove(const struct FExampleArray& InArraySerializer);
	void PostReplicatedAdd(const struct FExampleArray& InArraySerializer);
	void PostReplicatedChange(const struct FExampleArray& InArraySerializer);
	
};

/** Step 2: You MUST wrap your TArray in another struct that inherits from FFastArraySerializer */
USTRUCT()
struct FExampleArray : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FExampleItemEntry> Items; /** Step 3: You MUST have a TArray named Items of the struct you made in step 1. */

	UPROPERTY()
	AMyActor* MyActor=nullptr;
	
	/** Step 4: Copy this, replace example with your names */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		bool bSuccess = FFastArraySerializer::FastArrayDeltaSerialize<FExampleItemEntry, FExampleArray>(Items, DeltaParms, *this);
		UE_LOG(LogTemp, Warning, TEXT("FExampleArray::NetDeltaSerialize: %d"), bSuccess);

		return bSuccess;
	}

	//~FFastArraySerializer contract
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract
};

/** Step 5: Copy and paste this struct trait, replacing FExampleArray with your Step 2 struct. */
template <>
struct TStructOpsTypeTraits<FExampleArray> : public TStructOpsTypeTraitsBase2<FExampleArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};


UCLASS()
class PROJECTGZ_API AMyActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyActor();
	UPROPERTY(Replicated)
	FExampleArray DeltaTest;

protected:


public:

	// Adding DOREPLIFETIME to the GetLifetimeReplicatedProps method
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(AMyActor, DeltaTest);
	}

UFUNCTION(BlueprintCallable)
	// Adding an element to the array
	void AddItem()
	{
		FExampleItemEntry a;
		a.ExampleFloatProperty = 3.14;
		a.ExampleIntProperty = 1234;
		DeltaTest.MarkItemDirty(DeltaTest.Items.Add_GetRef(a));
	}
	UFUNCTION(BlueprintCallable)
	// Modifying an element
	void ChangeItem(int32 ItemID)
	{
		if (DeltaTest.Items.Num() > ItemID)
		{
			DeltaTest.Items[ItemID].ExampleFloatProperty = 6.28;
			DeltaTest.Items[ItemID].ExampleIntProperty = 5678;
			DeltaTest.MarkItemDirty(DeltaTest.Items[ItemID]);
		}
	}
	UFUNCTION(BlueprintCallable)
	// Removing an element
	void RemoveLastItem()
	{
		if (DeltaTest.Items.Num() > 0)
		{
			DeltaTest.Items.RemoveAt(DeltaTest.Items.Num() - 1, 1);
			DeltaTest.MarkArrayDirty();
		}
	}
};
