#include "Character/Player/GZPlayerCharacter.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "AbilitySystem/GZInputGameplayAbilitySet.h"
#include "Camera/CameraComponent.h"
#include "Character/GZAimMotionComponent.h"
#include "Character/GZPawnFeatureComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/GZPlayerController.h"
#include "Player/GZPlayerState.h"
#include "ProjectGZ/ProjectGZ.h"
#include "UI/HUD/GZHUD.h"

AGZPlayerCharacter::AGZPlayerCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	AimMotionComponent = CreateDefaultSubobject<UGZAimMotionComponent>("AimMotion");
	const auto CMC = GetCharacterMovement();
	CMC->bOrientRotationToMovement = true;
	CMC->RotationRate = FRotator(0, DEFAULT_GZ_ROTATE_RATE_YAW, 0);
	CMC->bConstrainToPlane = true;
	CMC->bSnapToPlaneAtStart = true;
	CMC->bUseControllerDesiredRotation = false;
	CMC->GetNavAgentPropertiesRef().bCanCrouch = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = true;
	Camera->bUsePawnControlRotation = false;
}

void AGZPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	BindAnimLayerChangedEvent();
	OnAnimLayerChanged.Broadcast(AnimLayerSet->DefaultAnimLayerClass);
}

void AGZPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//called on the server
	InitializePawnFeature();
	UE_LOG(LogTemp, Log, TEXT("AGZPlayerCharacter::InitializePawnFeature called on the server"));
}

void AGZPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//called on the client
	InitializePawnFeature();
	UE_LOG(LogTemp, Log, TEXT("AGZPlayerCharacter::InitializePawnFeature called on the client"));
}

void AGZPlayerCharacter::PitchCamera(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void AGZPlayerCharacter::YawCamera(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

UGZAimMotionComponent* AGZPlayerCharacter::GetAimMotionComponent()
{
	return AimMotionComponent.Get();
}

void AGZPlayerCharacter::Strafe()
{
	bIsStrafing = true;
}

void AGZPlayerCharacter::Unstrafe()
{
	bIsStrafing = false;
}

bool AGZPlayerCharacter::IsStrafing()
{
	return bIsStrafing;
}

UGZAbilitySystemComponent* AGZPlayerCharacter::GetAbilitySystemComponent() const
{
	if (!UGZPawnFeature) return nullptr;
	return UGZPawnFeature->GetAbilitySystem();
}

UGZAttributeSet* AGZPlayerCharacter::GetAttributeSet() const
{
	if (!UGZPawnFeature) return nullptr;
	return UGZPawnFeature->GetAttributeSet();
}

void AGZPlayerCharacter::OnEquipmentTagChanged_Implementation(FGameplayTag EquipmentTag)
{
	if (!IsValid(AnimLayerSet))
	{
		Debug::Print(TEXT("OnEquipmentTagChanged: AnimLayerSet is Null!!"));
		return;
	}
	TSubclassOf<UAnimInstance> EquipmentAnimLayer = AnimLayerSet->GetAnimLayer(EquipmentTag);
	OnAnimLayerChanged.Broadcast(EquipmentAnimLayer);
}

void AGZPlayerCharacter::InitializePawnFeature()
{
	AGZPlayerState* GZPlayerState = GetPlayerState<AGZPlayerState>();
	check(GZPlayerState);
	UGZPawnFeature = GZPlayerState->GetPawnFeature();
	UGZPawnFeature->InitAbilityActorInfo(GZPlayerState, this);
	UGZPawnFeature->OnInitializePawnFeature();

	AGZPlayerController* GZPlayerController = Cast<AGZPlayerController>(GetController());
	if (GZPlayerController)
	{
		//init HUD
		if (AGZHUD* HUD = GZPlayerController->GetHUD<AGZHUD>())
		{
			HUD->InitializeOverlay(GZPlayerController, GZPlayerState, UGZPawnFeature->GetAbilitySystem(), UGZPawnFeature->GetAttributeSet());
		}
	}
}

