// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

class AWeapon;
class ABlasterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	friend class ABlasterCharacter;
	void EquipWeapon(AWeapon* WeaponToEquip);


protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bADSPressed);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bADSPressed);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireButtonActive(bool bFireNow);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);


	void TraceUnderCrosshair(FHitResult& TraceHitResult);

private:	
	ABlasterCharacter* Character;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float AimWalkingSpeed;

	UPROPERTY(EditAnywhere)
	float BaseWalkingSpeed;

	bool bActivateFiring;
	
};
