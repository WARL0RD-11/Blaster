// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere)
	TObjectPtr < USpringArmComponent>	SpringArm;

	UPROPERTY(EditAnywhere)
	TObjectPtr <UCameraComponent>		Camera; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent>	OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappedWeapon)
	AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappedWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* m_Combat;

	UFUNCTION(Server, Reliable)
	void ServerEquippingFunc();  //RPC for Callback function that is being accessed in the Player Controller for equuipping

protected:
	virtual void BeginPlay() override;


public:	
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;

	void SetOverlappedWeapon(AWeapon* Weapon);

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	//controller functions
	void EquippingFunc(); //Callback function that is being accessed in the Player Controller for equuipping.


	bool IsWeaponEquipped();
	


};
