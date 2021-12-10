#pragma once
#include <random>
#include "PxPhysicsAPI.h"
class RandomGenerator
{
	std::mt19937 prng;
	RandomGenerator() : prng{ std::random_device{}() } {};


public:

	RandomGenerator(const RandomGenerator&) = delete;
	RandomGenerator& operator=(const RandomGenerator&) = delete;

	static RandomGenerator& get();

	int next();
	int next(int min, int max);

	physx::PxVec3 randomVec3(int min, int max);
	physx::PxVec3 randomPosInZone(int zone);

};

