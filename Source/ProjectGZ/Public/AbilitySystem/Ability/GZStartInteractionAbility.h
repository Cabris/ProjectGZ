#pragma once

#include "CoreMinimal.h"
#include "GZGameplayAbility.h"
#include "GZStartInteractionAbility.generated.h"

UCLASS()
class PROJECTGZ_API UGZStartInteractionAbility : public UGZGameplayAbility
{
	GENERATED_BODY()
public:
	UGZStartInteractionAbility();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
protected:
	// ===== 可調參數 =====
	UPROPERTY(EditDefaultsOnly, Category="Trace")
	float TraceRadius = 250.f;

	UPROPERTY(EditDefaultsOnly, Category="Trace")
	float TraceDistance = 600.f;

	UPROPERTY(EditDefaultsOnly, Category="Trace", meta=(ClampMin="0.01"))
	float TraceInterval = 0.08f; // 12.5Hz

	UPROPERTY(EditDefaultsOnly, Category="Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, Category="Score")
	float AimWeight = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category="Score")
	float DistanceWeight = 0.3f;
private:
	FTimerHandle TraceTimerHandle;
	TWeakObjectPtr<AActor> ActiveActor;
	TWeakObjectPtr<AActor> LastActiveActor;

	TArray<FHitResult> Hits;

	void TickTrace();
	bool GetViewPoint(FVector& OutLoc, FVector& OutDir) const;
	float ScoreCandidate(const FVector& ViewLoc, const FVector& ViewDir, const FVector& TargetLoc) const;

	void BeginFocus(AActor* NewActor);
	void EndFocus(AActor* OldActor);

	void AttemptInteraction_Internal();
	
};
