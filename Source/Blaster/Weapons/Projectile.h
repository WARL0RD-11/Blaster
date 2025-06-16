
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	AProjectile();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(
		UPrimitiveComponent*	HitComp, 
		AActor*					OtherActor, 
		UPrimitiveComponent*	OtherComp, 
		FVector					NormalImpulse, 
		const FHitResult&		Hit
	);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHit();

private:	
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UParticleSystem> Tracer;

	TObjectPtr<class UParticleSystemComponent> TracerComponent;



	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ImpactParticles;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> ImpactSound;



};
