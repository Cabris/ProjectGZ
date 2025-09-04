#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "UObject/Object.h"
#include "Serialization/Archive.h"
#include "GZAbilityTargetData_SingleTargetHit.generated.h"


USTRUCT()
struct FGZAbilityTargetData_SingleTargetHit : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()

	FGZAbilityTargetData_SingleTargetHit(): FireIndex(INDEX_NONE), ClientTimestamp(0)
	{
	}

	//加入自定義屬性:
	//用以區分多個hit point，於server side Validate 射擊次數
	UPROPERTY()
	int32 FireIndex;
	//用以區分射擊時間，避免重複
	UPROPERTY()
	float ClientTimestamp;


	//加入NetSerialize函式
	/** Serialize for networking, handles polymorphism */
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		if (!Super::NetSerialize(Ar, Map, bOutSuccess))
		{
			return false;
		}

		// 序列化自定義數據
		Ar << FireIndex;
		Ar << ClientTimestamp;
		return true;
	}

	/** Returns the serialization data, must always be overridden */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGZAbilityTargetData_SingleTargetHit::StaticStruct();
	}
};

//用來模板的類型特徵來讓自定義的結構序列化，WithNetSerializer用來啟用NetSerializer 
template <>
struct TStructOpsTypeTraits<FGZAbilityTargetData_SingleTargetHit> : public TStructOpsTypeTraitsBase2<FGZAbilityTargetData_SingleTargetHit>
{
	enum
	{
		WithNetSerializer = true // For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};
