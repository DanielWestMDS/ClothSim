// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class GD2P02_CLOTHSIM_API ClothParticle
{
public:
	/// <summary>
	/// Constructor. Sets the position of this particle
	/// </summary>
	/// <param name="Position"></param>
	ClothParticle(FVector Position);

	/// <summary>
	/// Destructor
	/// </summary>
	~ClothParticle();

	/// <summary>
	/// Adds a constraint
	/// </summary>
	/// <param name="_Constraint"></param>
	void AddConstraint(class ClothConstraint* _Constraint);

	bool SharesConstraint(ClothParticle* _otherParticle);

	bool GetPinned() { return m_bPinned; };
	void SetPinned(bool _pinned) { m_bPinned = _pinned; };

	TArray<class ClothConstraint*> GetConstraints() { return Constraints; };

	FVector GetPosition() { return Position; };

	void SetPosition(FVector _Position) { Position = _Position; };

	/// <summary>
	/// Changes the position of this particle by the vector offset parameter
	/// </summary>
	/// <param name="_offset"></param>
	void OffsetPosition(FVector _offset);

	void AddForce(FVector _force);

	void Update(float dt);

private:
	FVector Position = { 0, 0, 0 };
	FVector OldPosition = { 0, 0, 0 };
	float OldDeltaTime = -1.0f;
	FVector Acceleration = { 0, 0, 0 };

	TArray<class ClothConstraint*> Constraints;

	bool m_bPinned = false;

	float Damping = 0.0f;
};
