#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActiveGameplayEffectHandle.h"
#include "GZEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectCondition:uint8
{
	OnOverlap,
	OnEndOverlap,
	Omit
};

USTRUCT(BlueprintType)
struct FEffectParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	TSubclassOf<UGameplayEffect> EffectClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	EEffectCondition ApplyCondition = EEffectCondition::Omit;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	EEffectCondition RemoveCondition = EEffectCondition::Omit;
};

UCLASS()
class PROJECTGZ_API AGZEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AGZEffectActor();
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* EffectedTarget, FEffectParams& InEffectParams, float Level);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	bool bDestroyOnEffectRemoval = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	TArray<FEffectParams> Effects;
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects", meta = (ClampMin = "1", ClampMax = "10"))
	float ActorLevel = 1;
private:
};
