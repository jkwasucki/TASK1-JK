// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/DamageableInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "TargetDummy.generated.h"

UCLASS()
class TASKN1_API ATargetDummy : public AActor, public IDamageableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATargetDummy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UCapsuleComponent* Collider;
	
	
	UPROPERTY(VisibleAnywhere)
	float Health = 9999.f;

public:	

	virtual void TakeDamage_Implementation(float Value) override;
};
