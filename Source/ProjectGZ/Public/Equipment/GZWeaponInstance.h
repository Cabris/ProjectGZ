#pragma once
#include "CoreMinimal.h"
#include "GZEquipmentInstance.h"
#include "WeaponConfig.h"
#include "GZWeaponInstance.generated.h"

USTRUCT(BlueprintType)
struct FAttackFilter
{
	GENERATED_BODY()
	// 用哪個通道做 LineTrace/Sweep（預設拿 Visibility）
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	// 是否用複雜碰撞
	UPROPERTY(EditAnywhere)
	bool bTraceComplex = false;

	// 忽略 Instigator 以及它的「所有附掛的子 Actor」
	// （包含 ChildActorComponent 產生的 Child Actors）
	UPROPERTY(EditAnywhere)
	bool bIgnoreInstigatorAndAttachments = true;

	// 這次射擊的「發射者」；用來忽略自己
	UPROPERTY()
	TWeakObjectPtr<const AActor> Instigator = nullptr;

	// 額外忽略的Actor（武器 Actor、臨時特效 Actor）
	UPROPERTY()
	TArray<TWeakObjectPtr<const AActor>> ExtraIgnoredActors;

	// 用「物件類型」而不是 Channel
	// FCollisionObjectQueryParams ObjectTypes = FCollisionObjectQueryParams::AllDynamicObjects;
};

struct FFireParams
{
	FVector Origin = FVector::Zero();
	FVector Direction = FVector::Zero();
	float InitialSpeed = 0;
	float MaxRange = 0;
	int32 FireIndex = 0;
	FAttackFilter Filter;
#ifdef  WITH_EDITOR
	bool bDrawDebug = false;
#endif
};

struct FFireResult
{
	bool bIsValid = false;
	bool bIsHit = false;
	FHitResult Hit;
	FVector EndLocation = FVector::ZeroVector;
};

//base c++ class for ranged/melee weapon
UCLASS(Blueprintable, BlueprintType)
class PROJECTGZ_API UGZWeaponInstance : public UGZEquipmentInstance
{
	GENERATED_BODY()

public:
	const FWeaponConfig& GetConfig() const { return WeaponConfig; }
	static void BuildCollisionParams(const FAttackFilter& Filter, FCollisionQueryParams& OutQuery);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void FireWeapon(const FHitResult& HitResult);
protected:
	UPROPERTY(EditDefaultsOnly, Category="Equipment|Weapon")
	FWeaponConfig WeaponConfig;
};
