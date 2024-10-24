// Fill out your copyright notice in the Description page of Project Settings.


#include "Cloth.h"
#include "KismetProceduralMeshLibrary.h"
#include "ClothParticle.h"
#include "ClothConstraint.h"


// Sets default values
ACloth::ACloth()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ClothMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Procedural Mesh");
	ClothMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACloth::BeginPlay()
{
	Super::BeginPlay();

	ClothMesh->SetMaterial(0, ClothMaterial);
	
	CreateParticles();
	CreateConstraints();

	GenerateMesh();

	GetWorldTimerManager().SetTimer(UpdateTimer, this, &ACloth::Update, TimeStep, true);
}

void ACloth::CreateParticles()
{
	// -1 cause post fence problem
	HorizDist = ClothWidth / (NumHorizParticles - 1);
	VertDist = ClothHeight / (NumVertParticles - 1);

	FVector StartPos(0);
	StartPos.X = -ClothWidth / 2;
	StartPos.Z = ClothHeight / 2;

	// vertical first
	for (int Vert = 0; Vert < NumVertParticles; Vert++)
	{
		TArray<ClothParticle*> ParticleRow;

		for (int Horiz = 0; Horiz < NumHorizParticles; Horiz++)
		{
			FVector ParticlePos = { StartPos.X + Horiz * HorizDist, StartPos.Y, StartPos.Z - Vert * VertDist };

			ClothParticle* NewParticle = new ClothParticle(ParticlePos);

			// sets the first row every 5th particle to be pinned
			bool bShouldPin = Vert == 0 && Horiz % 5 == 0;
			NewParticle->SetPinned(bShouldPin);

			ParticleRow.Add(NewParticle);
		}

		Particles.Add(ParticleRow);
	}
}

void ACloth::CreateConstraints()
{
	// vertical first
	for (int Vert = 0; Vert < NumVertParticles; Vert++)
	{
		for (int Horiz = 0; Horiz < NumHorizParticles; Horiz++)
		{
			// check constraint is not on the very end
			if (Vert < NumVertParticles - 1)
			{
				// make a vertical constraint
				ClothConstraint* NewConstraint = new ClothConstraint(Particles[Vert][Horiz], Particles[Vert + 1][Horiz]);
				Constraints.Add(NewConstraint);

				Particles[Vert][Horiz]->AddConstraint(NewConstraint);
				Particles[Vert + 1][Horiz]->AddConstraint(NewConstraint);
			}

			// check constraint is not on the very end
			if (Horiz < NumHorizParticles - 1)
			{
				// make a horizontal constraint
				ClothConstraint* NewConstraint = new ClothConstraint(Particles[Vert][Horiz], Particles[Vert][Horiz + 1]);
				Constraints.Add(NewConstraint);

				Particles[Vert][Horiz]->AddConstraint(NewConstraint);
				Particles[Vert][Horiz + 1]->AddConstraint(NewConstraint);
			}
		}

	}
}

void ACloth::GenerateMesh()
{
	ClothVertices.Reset();
	ClothTriangles.Reset();
	ClothNormals.Reset();
	ClothTangents.Reset();
	ClothUVs.Reset();
	ClothColors.Reset();

	for (int Vert = 0; Vert < NumVertParticles; Vert++)
	{
		for (int Horiz = 0; Horiz < NumHorizParticles; Horiz++)
		{
			ClothVertices.Add(Particles[Vert][Horiz]->GetPosition());

			ClothColors.Add(FLinearColor::Black);

			ClothUVs.Add(FVector2D(float(Horiz) / (NumHorizParticles - 1), float(Vert) / (NumVertParticles - 1)));
		}
	}

	for (int Vert = 0; Vert < NumVertParticles - 1; Vert++)
	{
		for (int Horiz = 0; Horiz < NumHorizParticles - 1; Horiz++)
		{
			TryCreateTriangles(Particles[Vert][Horiz], Particles[Vert][Horiz + 1], 
				Particles[Vert + 1][Horiz], Particles[Vert + 1][Horiz + 1], Vert * NumHorizParticles + Horiz);
		}
	}

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(ClothVertices, ClothTriangles, ClothUVs, ClothNormals, ClothTangents);

	// NO CHEATING !!!! END PARAMETER MUST BE FALSE !!! WE DO COLLISIONS OURSELVES AROUND HERE !!!
	ClothMesh->CreateMeshSection_LinearColor(0, ClothVertices, ClothTriangles, ClothNormals, ClothUVs, ClothColors, ClothTangents, false);
}

void ACloth::TryCreateTriangles(ClothParticle* _topLeft, ClothParticle* _topRight, ClothParticle* _bottomLeft, ClothParticle* _bottomRight, int _topLeftIndex)
{
	int TopLeftIndex = _topLeftIndex;
	int TopRightIndex = _topLeftIndex + 1;
	int BottomLeftIndex = _topLeftIndex + NumHorizParticles;
	int BottomRightIndex = _topLeftIndex + NumHorizParticles + 1;

	if (_topLeft->SharesConstraint(_topRight) && _topLeft->SharesConstraint(_bottomLeft))
	{
		// clockwise
		ClothTriangles.Add(TopLeftIndex);
		ClothTriangles.Add(TopRightIndex);
		ClothTriangles.Add(BottomLeftIndex);

		if (_bottomRight->SharesConstraint(_topRight) && _bottomRight->SharesConstraint(_bottomLeft))
		{
			// clockwise
			ClothTriangles.Add(TopRightIndex);
			ClothTriangles.Add(BottomRightIndex);
			ClothTriangles.Add(BottomLeftIndex);
		}
	}
	// edge case for tearing
	else if (_bottomLeft->SharesConstraint(_topLeft) && _bottomLeft->SharesConstraint(_bottomRight))
	{
		// clockwise
		ClothTriangles.Add(BottomLeftIndex);
		ClothTriangles.Add(TopLeftIndex);
		ClothTriangles.Add(BottomRightIndex);

		if (_topRight->SharesConstraint(_bottomRight) && _topRight->SharesConstraint(_topLeft))
		{
			// clockwise
			ClothTriangles.Add(TopRightIndex);
			ClothTriangles.Add(BottomRightIndex);
			ClothTriangles.Add(TopLeftIndex);
		}
	}
}

void ACloth::Destroyed()
{
	for (int Vert = 0; Vert < Particles.Num(); Vert++)
	{
		for (int Horiz = 0; Horiz < Particles[Vert].Num(); Horiz++)
		{
			delete Particles[Vert][Horiz];
		}
	}

	for (auto constraint : Constraints)
	{
		delete constraint;
	}

	Particles.Empty();
	Constraints.Empty();

	Super::Destroyed();
}

void ACloth::Update()
{
	// update all constraints
	for (auto iter : Constraints)
	{
		iter->Update(TimeStep);
	}

	// update all particles
	for (int Vert = 0; Vert < Particles.Num(); Vert++)
	{
		for (int Horiz = 0; Horiz < Particles[Vert].Num(); Horiz++)
		{
			Particles[Vert][Horiz]->Update(TimeStep);
		}
	}
}

// Called every frame
void ACloth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GenerateMesh();
}

