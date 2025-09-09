#include "Actor//GZWeaponSpawner.h"
#include "Character/GZPawnFeatureComponent.h"
#include "Data/GZPickupDefinitionDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/Pawn.h"
#include "Interfactions/GZPawnFeatureInterface.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Inventory/GZInventoryManagerComponent.h"


// Sets default values
AGZWeaponSpawner::AGZWeaponSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bCanPickup = true;
	CoolDownTime = 0;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(GetRootComponent());

	if (IsValid(PickupItemDefinition))
	{
		CoolDownTime = PickupItemDefinition->SpawnCoolDownSeconds;
		WeaponMesh->SetStaticMesh(PickupItemDefinition->DisplayMesh);
		WeaponMesh->SetRelativeTransform(PickupItemDefinition->DisplayMeshTransformOffset);
	}
}

void AGZWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();
	OnCoolDownTimerComplete();
	bCanPickup = true;
	StartTimer();
}

void AGZWeaponSpawner::AttemptPickUpWeapon(APawn* ReceivingPawn)
{
	if (ReceivingPawn && IsValid(ReceivingPawn) && IsValid(PickupItemDefinition) && bCanPickup)
	{
		TSubclassOf<UGZInventoryItemDefinition> ItemDefinition = PickupItemDefinition->PickupDefinitionClass;
		if (!IsValid(ItemDefinition))return;
		if (GiveWeapon(ItemDefinition, ReceivingPawn))
		{
			bCanPickup = false;
			ResetCoolDown();
			StartTimer();
		}
	}
}

bool AGZWeaponSpawner::GiveWeapon(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefinitionClass, APawn* ReceivingPawn)
{
	IGZPawnFeatureInterface* PawnFeature = Cast<IGZPawnFeatureInterface>(ReceivingPawn);
	if (!PawnFeature)
	{
		UE_LOG(LogTemp, Error, TEXT("Can not cast ReceivingPawn to IGZPawnFeatureInterface"));
		return false;
	}
	auto InventoryManager = PawnFeature->GetPawnFeature()->GetInventoryManager();
	if (!InventoryManager)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryManager is Null"));
		return false;
	}

	UGZInventoryItemInstance* ItemInstance = InventoryManager->AddItemDefToInventory(ItemDefinitionClass);
	UE_LOG(LogTemp, Warning, TEXT("ItemInstance: %p"), ItemInstance);
	return ItemInstance != nullptr;
}

void AGZWeaponSpawner::StartTimer()
{
	GetWorldTimerManager().SetTimer(CoolDownTimerHandle, this, &ThisClass::OnCoolDownTimerComplete, CoolDownTime);
}

void AGZWeaponSpawner::ResetCoolDown()
{
	GetWorldTimerManager().ClearTimer(CoolDownTimerHandle);
}

void AGZWeaponSpawner::OnCoolDownTimerComplete()
{
	bCanPickup = true;
	PlaySpawnerReadyFx();
	ResetCoolDown();
}

void AGZWeaponSpawner::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bCanPickup);
}
#if WITH_EDITOR
void AGZWeaponSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
