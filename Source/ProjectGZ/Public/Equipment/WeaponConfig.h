#pragma once
#include "CoreMinimal.h"
#include "WeaponConfig.generated.h"

UENUM(BlueprintType)
enum class EWeaponClass : uint8
{
	Melee,
	Ranged,
	Throwable
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Semi, // 單發
	Auto, // 連發（按住）
	HoldRelease // 按住蓄力，放開才觸發（弓/投擲/戰槌）
};

//common attributes for ranged/melee weapon
USTRUCT(BlueprintType)
struct FWeaponConfig
{
	GENERATED_BODY()
	//通用
	UPROPERTY(EditDefaultsOnly, Category="Common")
	EWeaponClass WeaponClass = EWeaponClass::Ranged;

	UPROPERTY(EditDefaultsOnly, Category="Common")
	float BaseDamage = 25.f;

	UPROPERTY(EditDefaultsOnly, Category="Common")
	EFireMode FireMode = EFireMode::Semi;

	// 以「單發間隔（秒）」為權威，RPM = 60 / FireInterval
	UPROPERTY(EditDefaultsOnly, Category="Common", meta=(ClampMin="0.02"))
	float FireInterval = 0.10f; // 600RPM

	UPROPERTY(EditDefaultsOnly, Category="Common")
	FName MuzzleSocketName = TEXT("weapon_r_muzzle");

	UPROPERTY(EditDefaultsOnly, Category="Common")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	//遠程
	UPROPERTY(EditDefaultsOnly, Category="Ranged", meta=(EditCondition="WeaponClass == EWeaponClass::Ranged"))
	int32 MagazineSize = 30;

	//Hitscan 射程in公尺
	UPROPERTY(EditDefaultsOnly, Category="Ranged", meta=(EditCondition="WeaponClass == EWeaponClass::Ranged && !bUseProjectile"))
	float MaxRangeMeters = 10000.f;

	UPROPERTY(EditDefaultsOnly, Category="Ranged", meta=(EditCondition="WeaponClass == EWeaponClass::Ranged"))
	bool bUseProjectile = false;

	UPROPERTY(EditDefaultsOnly, Category="Ranged", meta=(EditCondition="WeaponClass == EWeaponClass::Ranged && bUseProjectile"))
	TSoftClassPtr<AActor> ProjectileClass;

	//投射物初速/重力單位 cm/s
	UPROPERTY(EditDefaultsOnly, Category="Ranged", meta=(EditCondition="WeaponClass == EWeaponClass::Ranged && bUseProjectile"))
	float InitialSpeed = 6000.f;

	UPROPERTY(EditDefaultsOnly, Category="Ranged", meta=(EditCondition="WeaponClass == EWeaponClass::Ranged && bUseProjectile"))
	float GravityScale = 1.0f;

	//近戰
	UPROPERTY(EditDefaultsOnly, Category="Melee", meta=(EditCondition="WeaponClass == EWeaponClass::Melee"))
	float MeleeRange = 250.f; // 觸達距離（cm）

	UPROPERTY(EditDefaultsOnly, Category="Melee", meta=(EditCondition="WeaponClass == EWeaponClass::Melee"))
	float MeleeRadius = 60.f; // 掃掠半徑（cm）

	//投擲
	UPROPERTY(EditDefaultsOnly, Category="Throwable", meta=(EditCondition="WeaponClass == EWeaponClass::Throwable"))
	float FuseTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category="Throwable", meta=(EditCondition="WeaponClass == EWeaponClass::Throwable"))
	float ExplosionRadius = 350.f; // cm

	// 擊發 Notify 的正規化時間（0~1），用來計算 PlayRate 對齊射速
	UPROPERTY(EditDefaultsOnly, Category="Anim", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ShotNotifyNormalizedTime = 0.2f;

	//Camera pose override Tag
	UPROPERTY(EditDefaultsOnly, Category="Common|Camera")
	FGameplayTag CameraPoseOverrideTag;
	//Time for Camera recover to default pose
	UPROPERTY(EditDefaultsOnly, Category="Common|Camera")
	float CameraPoseRecoveryTime;
	
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bDrawDebug=false;
	// 小工具
	float GetRPM() const { return FireInterval > 0.f ? 60.f / FireInterval : 0.f; }
};
