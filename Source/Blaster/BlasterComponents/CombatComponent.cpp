


#include "CombatComponent.h"
#include "Blaster/Weapons/Weapon.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Character/BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkingSpeed = 650.f;
	AimWalkingSpeed = 350.f;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkingSpeed;
	}
	GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &UCombatComponent::TryInitializeHUD, 0.1f, true);
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	SetHUDCrosshairs(DeltaTime);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}


void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{

	if (Character == nullptr || WeaponToEquip == nullptr) 
		return;
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}

	EquippedWeapon->SetOwner(Character);
	//EquippedWeapon->ShowPickupWidget(false);
}

void UCombatComponent::SetAiming(bool bADSPressed)
{
	if (Character->HasAuthority())
	{
		bAiming = bADSPressed;
		Character->GetCharacterMovement()->MaxWalkSpeed = bADSPressed ? AimWalkingSpeed : BaseWalkingSpeed;
	}
	else
	{
		ServerSetAiming(bADSPressed);
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bADSPressed)
{
	bAiming = bADSPressed;
	Character->GetCharacterMovement()->MaxWalkSpeed = bADSPressed ? AimWalkingSpeed : BaseWalkingSpeed;
}

void UCombatComponent::FireButtonActive(bool bFireNow)
{
	bActivateFiring = bFireNow;
	if (bActivateFiring)
	{
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		ServerFire(HitResult.ImpactPoint);
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}

	if (Character)
	{
		EquippedWeapon->Fire(TraceHitTarget);
	}
}


void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End,
			ECollisionChannel::ECC_Visibility);

		/*if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}
		else
		{
			HitTarget = TraceHitResult.ImpactPoint;
			DrawDebugSphere(GetWorld(), TraceHitResult.ImpactPoint, 12.f, 12, FColor::Red);
		}*/
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	//if (Character == nullptr || Character->Controller == nullptr)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Character or Controller is null"));
	//	return;
	//} 

	//PlayerController = PlayerController == nullptr ? Cast<ABlasterPlayerController>(Character->Controller)
	//	: PlayerController;

	//if (!PlayerController)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("PlayerController cast failed"));
	//	return;
	//}

	//if (PlayerController)
	//{
	//	HUD = HUD == nullptr ? Cast<ABlasterHUD>(PlayerController->GetHUD()) : HUD;

		if (!bHUDInitialized)
		{
			UE_LOG(LogTemp, Warning, TEXT("HUD cast failed"));
			return;
		}

		FHUDPackage HUDPackage;

		if (EquippedWeapon)
		{
			HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
			HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
			HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
			HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
		}
		else
		{
			HUDPackage.CrosshairsCenter = nullptr;
			HUDPackage.CrosshairsTop = nullptr;
			HUDPackage.CrosshairsBottom = nullptr;
			HUDPackage.CrosshairsLeft = nullptr;
			HUDPackage.CrosshairsRight = nullptr;
		}


		FVector2D WalkingSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
		FVector2D VelocityMultiplier(0.f, 1.0f);
		FVector Velocity = Character->GetVelocity();
		Velocity.Z = 0;

		CrosshairVelocitySpreadFactor = FMath::GetMappedRangeValueClamped(WalkingSpeedRange, VelocityMultiplier, 
			Velocity.Size());

		if (Character->GetCharacterMovement()->IsFalling())
		{
			CrosshairInAirSpread = FMath::FInterpTo(CrosshairInAirSpread, 2.0f, DeltaTime, 2.0f);
		}
		else
		{
			CrosshairInAirSpread = FMath::FInterpTo(CrosshairInAirSpread, 0.0f, DeltaTime, 0.0f);
		}

		HUDPackage.CrosshairSpread = CrosshairVelocitySpreadFactor + CrosshairInAirSpread;
		HUD->SetHUDPackage(HUDPackage);
}

void UCombatComponent::TryInitializeHUD()
{
	//Custom Lazy Load of HUD to manage Exceptions

	if (!Character || !Character->Controller) return;

	PlayerController = Cast<ABlasterPlayerController>(Character->Controller);
	if (!PlayerController) return;

	HUD = Cast<ABlasterHUD>(PlayerController->GetHUD());
	if (HUD)
	{
		bHUDInitialized = true;
		GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);
	}
}
