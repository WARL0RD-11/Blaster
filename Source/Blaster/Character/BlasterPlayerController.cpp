#include "BlasterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "BlasterCharacter.h"

ABlasterPlayerController::ABlasterPlayerController()
{
	bReplicates = true;

}

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Only setup input on locally controlled clients
	if (IsLocalController())
	{
		check(BlasterContext);

		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

			if (Subsystem)
			{
				Subsystem->AddMappingContext(BlasterContext, 0);
			}
		}
	}
}

void ABlasterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalController()) return;

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
			&ABlasterPlayerController::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
			&ABlasterPlayerController::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this,
			&ABlasterPlayerController::Jump);

		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this,
			&ABlasterPlayerController::EquipButtonPressed);
			
	}
}

void ABlasterPlayerController::Move(const FInputActionValue& InputActionValue)
{
	// Client-side only
	if (!IsLocalController()) return;

	const FVector2D MovementDirection = InputActionValue.Get<FVector2D>();
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	if (APawn* ControlledPawn = GetPawn())
	{
		// Call to server to move, or rely on replicated CharacterMovement
		ControlledPawn->AddMovementInput(RightDirection, MovementDirection.X);
		ControlledPawn->AddMovementInput(ForwardDirection, MovementDirection.Y);
	}
}

void ABlasterPlayerController::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookDirection = InputActionValue.Get<FVector2d>();
	
	if (APawn* ControlledPawn = GetPawn())
	{
		// Call to server to move, or rely on replicated CharacterMovement
		ControlledPawn->AddControllerPitchInput(LookDirection.Y);
		ControlledPawn->AddControllerYawInput(LookDirection.X);
	}
}

void ABlasterPlayerController::Jump()
{
	if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (!IsLocalController())
		{
			return;
		}
		
		ControlledCharacter->Jump();
		
	}
}

void ABlasterPlayerController::EquipButtonPressed()
{
	if (!IsLocalController())
	{
		return;
	}
	
	if (ABlasterCharacter* BControlledCharacter = Cast<ABlasterCharacter>(GetPawn()))
	{
		if (BControlledCharacter->HasAuthority() && IsLocalPlayerController())
		{
			BControlledCharacter->EquippingFunc();
		}
	}
}


