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
		+ ((Position - OldPosition) * ((1.0f -Damping) * (dt / OldDeltaTime)))
		+ (Acceleration * dt * ((dt + OldDeltaTime) * 0.5f));
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
