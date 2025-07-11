// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	TObjectPtr<class UTexture2D> CrosshairsCenter;
	TObjectPtr<class UTexture2D> CrosshairsTop;
	TObjectPtr<class UTexture2D> CrosshairsBottom;
	TObjectPtr<class UTexture2D> CrosshairsLeft;
	TObjectPtr<class UTexture2D> CrosshairsRight;
	float CrosshairSpread;

};

UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; };
private: 
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread);

	float CrosshairSpreadMax = 16.0f;
	
};
