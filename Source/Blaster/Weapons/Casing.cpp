// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"


ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShellMesh"));
	SetRootComponent(ShellMesh);

}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
}

