// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class GD2P02_CLOTHSIM_API ClothConstraint
{
public:
	/// <summary>
	/// Constructor. Sets the resting distance between two particles and initialises those two particles within the class.
	/// </summary>
	/// <param name="_particleA"></param>
	/// <param name="_particleB"></param>
	ClothConstraint(class ClothParticle* _particleA, ClothParticle* _particleB);

	/// <summary>
	/// Destructor.
	/// </summary>
	~ClothConstraint();

	/// <summary>
	/// Updates the positions of particles A and B
	/// </summary>
	/// <param name="_deltaTime"></param>
	void Update(float _deltaTime);

private:

	ClothParticle* ParticleA = nullptr;
	ClothParticle* ParticleB = nullptr;

	float RestingDistance;


};
