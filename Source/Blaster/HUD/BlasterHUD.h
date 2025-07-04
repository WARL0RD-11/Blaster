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
	
};
