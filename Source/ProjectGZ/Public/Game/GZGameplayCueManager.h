#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "GZGameplayCueManager.generated.h"


UCLASS()
class PROJECTGZ_API UGZGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()
public:
	static UGZGameplayCueManager* Get();
	void StartInitialLoading();
protected:
private:
};
