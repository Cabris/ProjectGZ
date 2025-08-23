#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GZPawnComponent.generated.h"

class UGZPawnFeatureComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTGZ_API UGZPawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGZPawnComponent();
protected:
	UGZPawnFeatureComponent* GetPawnFeature() const;
};
