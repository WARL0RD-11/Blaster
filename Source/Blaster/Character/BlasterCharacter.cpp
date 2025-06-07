


#include "BlasterCharacter.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapons/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"


// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->TargetArmLength = 600.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(GetMesh());

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	m_Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	m_Combat->SetIsReplicated(true);


}


void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (m_Combat)
	{
		m_Combat->Character = this;
	}
}


void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	RemoteRole = GetRemoteRole();
	FString CRole;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		CRole = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		CRole = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		CRole = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		CRole = FString("None");
		break;
	}
	RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *CRole);
	
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlasterCharacter::SetOverlappedWeapon(AWeapon* Weapon)
{
	
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(false);
		}
	}

	OverlappingWeapon = Weapon;

	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}


void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}


void ABlasterCharacter::OnRep_OverlappedWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::EquippingFunc()
{
	if (m_Combat)
	{
		if (HasAuthority())
		{
			m_Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquippingFunc();
		}
	}
}

void ABlasterCharacter::ServerEquippingFunc_Implementation()
{
	if (m_Combat)
	{
		m_Combat->EquipWeapon(OverlappingWeapon);
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	if(m_Combat && m_Combat->EquippedWeapon)
		return true;
	else
		return false;
}

void ABlasterCharacter::AimingFunction(bool bAimPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("Blaster Character should aim - %d"), bAimPressed);
	if (m_Combat)
	{
		m_Combat->SetAiming(bAimPressed);
	}
}

bool ABlasterCharacter::IsADSActive()
{
	if (m_Combat && m_Combat->bAiming)
		return true;
	else
		return false;
}

void ABlasterCharacter::FiringFunction(bool bFirePressed)
{
	if (IsWeaponEquipped())
	{
		UE_LOG(LogTemp, Warning, TEXT("Role  - %s"), *RemoteRoleString);
		UE_LOG(LogTemp, Warning, TEXT("Should FIRE NOW - %d"), bFirePressed);
		m_Combat->FireButtonActive(bFirePressed);
	}
		
}
