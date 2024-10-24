// Fill out your copyright notice in the Description page of Project Settings.


#include "ClothConstraint.h"
#include "ClothParticle.h"

ClothConstraint::ClothConstraint(ClothParticle* _particleA, ClothParticle* _particleB)
{
	ParticleA = _particleA;
	ParticleB = _particleB;

	RestingDistance = FVector::Dist(ParticleA->GetPosition(), ParticleB->GetPosition());
	RestingDistance *= 0.7f;
}

ClothConstraint::~ClothConstraint()
{

}

void ClothConstraint::Update(float _deltaTime)
{
	FVector CurrentOffset = ParticleB->GetPosition() - ParticleA->GetPosition();

	FVector Correction = CurrentOffset * (1.0f - RestingDistance / CurrentOffset.Size());
	FVector HalfCorrection = Correction * 0.5;

	if (!ParticleA->GetPinned() && !ParticleB->GetPinned())
	{

		ParticleA->OffsetPosition(HalfCorrection);
		ParticleB->OffsetPosition(-HalfCorrection);
	}
	else if (!ParticleA->GetPinned())
	{
		ParticleA->OffsetPosition(Correction);
	}
	else if (!ParticleB->GetPinned())
	{
		ParticleB->OffsetPosition(-Correction);
	}
	// do nothing if both particles pinned

}