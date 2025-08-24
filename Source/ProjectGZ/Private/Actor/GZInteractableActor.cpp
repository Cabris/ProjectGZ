#include "Actor/GZInteractableActor.h"

#include "Components/SphereComponent.h"
#include "ProjectGZ/ProjectGZ.h"


// Sets default values
AGZInteractableActor::AGZInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	InteractCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractCollision"));
	InteractCollision->SetupAttachment(GetRootComponent());
	InteractCollision->InitSphereRadius(30.f);

	InteractCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractCollision->SetCollisionObjectType(ECC_WorldDynamic);

	InteractCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractCollision->SetCollisionResponseToChannel(InteractionChannel, ECR_Overlap);
	UE_LOG(LogTemp, Log, TEXT("AGZInteractableActor::Constructor(): %s"), *GetName());

}

// Called when the game starts or when spawned
void AGZInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	SetupComponents();
}

void AGZInteractableActor::SetupComponents()
{
}

void AGZInteractableActor::PostLoad()
{
	Super::PostLoad();
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		SetupComponents();
	}
}

FVector AGZInteractableActor::GetWorldPosition() const
{
	return GetActorLocation();
}

FText AGZInteractableActor::GetInteractionText() const
{
	return InteractionText;
}

bool AGZInteractableActor::IsInteractable() const
{
	return bIsInteractable;
}

const FGameplayTag& AGZInteractableActor::GetInteractAbilityTriggerTag()
{
	return InteractAbilityTriggerTag;
}

void AGZInteractableActor::DoInteract_Implementation()
{
	IGZInteractable::DoInteract_Implementation();
	GEngine->AddOnScreenDebugMessage(
		0, // Key
		3.0f, // Display time（0 表示直到下一次呼叫取代）
		FColor::Green, // 顏色
		FString("DoInteract_Implementation")
	);
}

void AGZInteractableActor::OnBeginFocus_Implementation()
{
	IGZInteractable::OnBeginFocus_Implementation();
	GEngine->AddOnScreenDebugMessage(
		0, // Key
		3.0f, // Display time（0 表示直到下一次呼叫取代）
		FColor::Green, // 顏色
		FString("OnBeginFocus_Implementation")
	);
}

void AGZInteractableActor::OnEndFocus_Implementation()
{
	IGZInteractable::OnEndFocus_Implementation();
	GEngine->AddOnScreenDebugMessage(
		0, // Key
		3.0f, // Display time（0 表示直到下一次呼叫取代）
		FColor::Green, // 顏色
		FString("OnEndFocus_Implementation")
	);
}