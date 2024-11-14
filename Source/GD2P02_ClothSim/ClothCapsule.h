// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClothCapsule.generated.h"

UCLASS()
class GD2P02_CLOTHSIM_API AClothCapsule : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClothCapsule();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
