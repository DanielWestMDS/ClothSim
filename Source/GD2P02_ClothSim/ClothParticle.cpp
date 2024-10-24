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

void ClothParticle::Update(float dt)
{
	FVector cachePosition = Position;
	Acceleration = { 2, 10, -100 };

	if (GetPinned())
	{
		Acceleration = { 0, 0, 0 };
	}

	// stop divide by 0
	if (OldDeltaTime <= 0.f)
	{
		OldDeltaTime = dt;
	}

	Position = Position
		+ ((Position - OldPosition) * (dt / OldDeltaTime))
		+ (Acceleration * dt * ((dt + OldDeltaTime) * 0.5f));
	Acceleration = { 0, 0, 0 };

	OldPosition = cachePosition;
	OldDeltaTime = dt;
}
