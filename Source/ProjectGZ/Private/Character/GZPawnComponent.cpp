#include "Character/GZPawnComponent.h"

#include "Character/GZPawnFeatureComponent.h"
#include "Interfactions/GZPawnFeatureInterface.h"


class IGZPawnFeatureInterface;

UGZPawnComponent::UGZPawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UGZPawnFeatureComponent* UGZPawnComponent::GetPawnFeature() const
{
	IGZPawnFeatureInterface* IPawnFeature = Cast<IGZPawnFeatureInterface>(GetOwner());
	if (!IPawnFeature) return nullptr;
	UGZPawnFeatureComponent* PawnFeature = IPawnFeature->GetPawnFeature();
	return PawnFeature;
}
