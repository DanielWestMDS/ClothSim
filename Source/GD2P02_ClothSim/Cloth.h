// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Cloth.generated.h"


class ClothParticle;

UCLASS()
class GD2P02_CLOTHSIM_API ACloth : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACloth();

	//UFUNCTION(BlueprintCallable, Category = "Cloth | Functions")
	//void ReleaseCloth();

	//UFUNCTION(BlueprintCallable, Category = "Cloth | Functions")
	//void ResetCloth();

	UFUNCTION(BlueprintCallable, Category = "Cloth | Functions")
		void ConstrictCloth(float _constrictedAmount);


	// reset cloth to original position
	UFUNCTION(BlueprintCallable, Category = "Cloth | Functions")
		void Reset();

	UFUNCTION(BlueprintCallable, Category = "Cloth | Functions")
		void ReleaseCloth();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// <summary>
	/// Runs on beginning. Creates the particles which make up the base of the mesh
	/// </summary>
	void CreateParticles();

	/// <summary>
	/// runs on beginning. Takes particles next to each other and connects them
	/// </summary>
	void CreateConstraints();

	/// <summary>
	/// Runs any time particles change position
	/// </summary>
	void GenerateMesh();

	FVector GetParticleNormal(int _xIndex, int _yIndex);

	/// <summary>
	/// Check if a quad of particles are connected and create triangles between them
	/// </summary>
	void TryCreateTriangles(class ClothParticle* _topLeft, ClothParticle* _topRight, ClothParticle* _bottomLeft, ClothParticle* _bottomRight, int _topLeftIndex);

	/// <summary>
	/// Go through all of the particles and update them
	/// </summary>
	void Update();

	void CalculateWindVector();

	void CheckForCollision();

	// clean up memory
	void Destroyed();




	// cloth setup 
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UProceduralMeshComponent* ClothMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UMaterial* ClothMaterial = nullptr;

	TArray<FVector> ClothVertices;
	TArray<int32> ClothTriangles;
	TArray<FVector> ClothNormals;
	TArray<FProcMeshTangent> ClothTangents;
	TArray<FVector2D> ClothUVs;
	TArray<FLinearColor> ClothColors;

	// grid of particles
	TArray<TArray<class ClothParticle*>> Particles;

	// list of all constraints
	TArray<class ClothConstraint*> Constraints;

	// Cloth properties
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	float ClothWidth = 200.0f; // in Centimeters
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	float ClothHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	int NumHorizParticles = 30;
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	int NumVertParticles = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	float HorizDist; // ClothWidth / NumHorizParticles
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	float VertDist; // ClothWidth / NumVertParticles
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	int NumHooks = 6;

	// simulation properties
	UPROPERTY(EditDefaultsOnly, Category = Simulation)
	int VerletIntegrationIterations = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Simulation)
	FVector WindVector = FVector(100.0f, 2000.0f, 100.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Simulation)
	FRotator WindRotation = { 0, 0, 0 };

	UPROPERTY(EditDefaultsOnly, Category = Simulation)
	float WindOscillationFrequency = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = Simulation)
	float WindOscillationFrequency2 = 2.27f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Simulation)
	float WindMultiplier = 1.0f;

	FTimerHandle UpdateTimer;
	float TimeStep = 0.016f; // 60 fps


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
