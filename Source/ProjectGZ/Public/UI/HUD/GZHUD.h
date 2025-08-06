#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GZHUD.generated.h"

class UGZUserWidget;
class UGZWidgetController;
class UGZAbilitySystemComponent;
class AGZPlayerState;
class UGZAttributeSet;
class AGZPlayerController;

UCLASS()
class PROJECTGZ_API AGZHUD : public AHUD
{
	GENERATED_BODY()

public:
	void InitializeOverlay(AGZPlayerController* PC, AGZPlayerState* PS,
	                       UGZAbilitySystemComponent* ASC, UGZAttributeSet* AS);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UGZUserWidget> OverlayWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UGZWidgetController> OverlayWidgetController;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGZUserWidget> OverlayWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGZWidgetController> OverlayWidgetControllerClass;

private:
};
