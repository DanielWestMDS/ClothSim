// Fill out your copyright notice in the Description page of Project Settings.


#include "ClothParticle.h"
#include "ClothConstraint.h"

ClothParticle::ClothParticle(FVector _position)
{
	Position = _position;
	OldPosition = _position;
}

ClothParticle::~ClothParticle()
{
}

void ClothParticle::AddConstraint(ClothConstraint* _Constraint)
{
	Constraints.Add(_Constraint);
}

bool ClothParticle::SharesConstraint(ClothParticle* _otherParticle)
{
	for (auto iter : _otherParticle->GetConstraints())
	{
		if (Constraints.Contains(iter)) 
		{
			return true;
		}
	}

	return false;
}

void ClothParticle::OffsetPosition(FVector _offset)
{
	Position += _offset;
}

void ClothParticle::AddForce(FVector _force)
{
	Acceleration += _force;
}

void ClothParticle::Update(float dt)
{
	FVector cachePosition = Position;
	//Acceleration = { 2, 10, -100 };

	if (GetPinned() || m_bOnGround)
	{
		Acceleration = { 0, 0, 0 };
		return;
	}

	// stop divide by 0
	if (OldDeltaTime <= 0.f)
	{
		OldDeltaTime = dt;
	}

	Position = Position
		+ (Position - OldPosition) * (1.0f -Damping * dt)
		+ (Acceleration * dt);

	Acceleration = { 0, 0, 0 };

	OldPosition = cachePosition;
	OldDeltaTime = dt;
}

void ClothParticle::CheckForGroundCollision(float _groundHeight)
{
	if (Position.Z <= _groundHeight)
	{
		Position.Z = _groundHeight;
		m_bOnGround = true;
	}
}

void ClothParticle::CheckForSphereCollision(FVector _sphereCenter, FVector _clothPosition)
{
	// radius is 100.f (not implementing dynamic collision fo dis one)
	float fRadius = 50.0f;

	FVector WorldPosition = Position + _clothPosition;

	FVector DistanceToSphere = WorldPosition - _sphereCenter;

	float fDistance = DistanceToSphere.Size();

	if (fDistance < fRadius)
	{
		// division by 0 edge case
		if (fDistance == 0.0f)
		{
			DistanceToSphere = FVector(1.0f, 0.0f, 0.0f);
			fDistance = 1.0f;
		}

		FVector Direction = DistanceToSphere / fDistance;

		// move the particle to the edge of the sphere
		FVector NewPosition = _sphereCenter + Direction * fRadius;

		Position = NewPosition - _clothPosition;

		Acceleration = { 0, 0, 0 };
	}
}

void ClothParticle::CheckForCapsuleCollision(FVector _capsuleCentre, FVector _clothPosition)
{
	// offset to account for the cloths world location
	FVector WorldPosition = Position + _clothPosition;

	// manually setting da radius
	float fCapsuleRadius = 50.f;
	// manually setting the start and end points (cuz idk how to get component locations from an actor in c++)
	FVector CapsuleStart = _capsuleCentre + FVector{ 0, 0, 50 };
	FVector CapsuleEnd = _capsuleCentre + FVector{ 0, 0, -50 };
	FVector CapsuleAxis = CapsuleEnd - CapsuleStart;
	float capsuleLength = CapsuleAxis.Size();

	// if start and end are the same then it's just a sphere
	if (capsuleLength == 0.0f)
	{
		CheckForSphereCollision(CapsuleStart, FVector::ZeroVector);
		return;
	}

	// normalise
	CapsuleAxis /= capsuleLength;

	FVector ParticleToStart = WorldPosition - CapsuleStart;

	// Project particle position onto the capsule axis
	float ProjectedLength = FVector::DotProduct(ParticleToStart, CapsuleAxis);

	// length shouldnt be more than the capsule length
	ProjectedLength = FMath::Clamp(ProjectedLength, 0.0f, capsuleLength);

	FVector ClosestPoint = CapsuleStart + CapsuleAxis * ProjectedLength;

	FVector VectorToCapsule = WorldPosition - ClosestPoint;

	// distance from the particle to the closest point on the capsule axis
	float fDistanceToCapsule = VectorToCapsule.Size();

	if (fDistanceToCapsule < fCapsuleRadius)
	{
		if (fDistanceToCapsule > 0)
		{
			VectorToCapsule = VectorToCapsule / fDistanceToCapsule;
		}

		// move the particle to the edge of the capsule
		FVector targetPosition = ClosestPoint + VectorToCapsule * fCapsuleRadius;

		Position = targetPosition - _clothPosition;

		Acceleration = { 0, 0, 0 };
	}
}

void ClothParticle::AddBurn(float _burnAmount)
{
	BurnAmount = FMath::Clamp(BurnAmount + _burnAmount, 0.0f, 1.0f);
}
