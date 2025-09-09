#pragma once

#include "CoreMinimal.h"
#include "GZUserWidget.h"
#include "GZWeaponSlotsWidget.generated.h"

class UListView;

UCLASS()
class PROJECTGZ_API UGZWeaponSlotsWidget : public UGZUserWidget
{
	GENERATED_BODY()
public:
protected:
	UPROPERTY(EditDefaultsOnly, Category = "WeaponSlot", meta = (BindWidget))
	TObjectPtr<UListView> ListView;
private:
	
};
