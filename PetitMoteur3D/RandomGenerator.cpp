#include "stdafx.h"
#include "RandomGenerator.h"


RandomGenerator& RandomGenerator::get() {
	static RandomGenerator singleton; //definition
	return singleton;
}

int RandomGenerator::next() {
	return std::uniform_int_distribution<int>{1, 100}(prng);
}

int RandomGenerator::next(int min, int max) {
	return std::uniform_int_distribution<int>{min, max}(prng);
}

physx::PxVec3 RandomGenerator::randomVec3(int min, int max) {
	const float x = static_cast<float>(std::uniform_int_distribution<int>{min, max}(prng));
	const float y = static_cast<float>(std::uniform_int_distribution<int>{min, max}(prng));
	const float z = static_cast<float>(std::uniform_int_distribution<int>{min, max}(prng));
	return std::move(physx::PxVec3(x, y, z));
}