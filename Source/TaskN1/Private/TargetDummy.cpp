// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/TargetDummy.h"
#include "Components/CapsuleComponent.h"

ATargetDummy::ATargetDummy()
{
	
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComponent->SetupAttachment(GetRootComponent());
	
	Collider = CreateDefaultSubobject<UCapsuleComponent>("Collider");
	Collider->SetupAttachment(MeshComponent);
}


void ATargetDummy::BeginPlay()
{
	Super::BeginPlay();
}

void ATargetDummy::TakeDamage_Implementation(float Value)
{
	Health -= Value;
}

