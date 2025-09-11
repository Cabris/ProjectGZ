#pragma once
#include "CoreMinimal.h"
#include "AbilitySystem/Ability/Combat/GZWeaponAbility.h"
#include "Components/ActorComponent.h"
#include "GZAimMotionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCameraRigTagChangedSingnature, const FGameplayTag&, CameraRigTag);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTGZ_API UGZAimMotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZAimMotionComponent();
	void AimAtRotation(FRotator rotator);
	void UnAim();
	bool GetIsAiming();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetCameraRigTag(FGameplayTag CameraPoseOverrideTag, float CameraPoseRecoveryTime, bool IsActivate);
	UPROPERTY(BlueprintAssignable)
	FCameraRigTagChangedSingnature OnCameraRigTagChanged;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim", ReplicatedUsing=OnRep_bIsAiming)
	bool bIsAiming;
	UPROPERTY(VisibleAnywhere, Category="Aim")
	TObjectPtr<APawn> AimingPawn;
	UPROPERTY(EditDefaultsOnly, Category="Aim")
	FGameplayTag DefaultCameraRigTag;
	
/*
	int32 ActiveCameraRig;
	float BoomLength;
	float BoomStep;
*/
private:
	UFUNCTION()
	void OnRep_bIsAiming(const bool& bOldIsAiming);
	UFUNCTION()
	void OnCameraRecovery();
	FGameplayTag CurrentActivateCameraRigTag;
	float CurrentActivateCameraPoseRecoveryTime;
	FTimerHandle RecoveryTimeerHandle;
};
