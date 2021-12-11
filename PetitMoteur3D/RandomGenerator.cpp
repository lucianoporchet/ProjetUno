#include "stdafx.h"
#include "RandomGenerator.h"
#include "SceneManager.h"


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

physx::PxVec3 RandomGenerator::randomPosInZone(int zone) {

	const float boxsize = SceneManager::get().getBoxSize() - 50.0f;
	const float z = static_cast<float>(std::uniform_int_distribution<int>{static_cast<int>(-boxsize / 2), static_cast<int>(boxsize / 2)}(prng));
	float x, y;
	switch (zone)
	{
	case 0:
		x = static_cast<float>(std::uniform_int_distribution<int>{static_cast<int>(-boxsize / 2), static_cast<int>(boxsize / 2)}(prng));
		y = static_cast<float>(std::uniform_int_distribution<int>{static_cast<int>(-boxsize / 2), static_cast<int>(boxsize / 2)}(prng));
		break;
	case 1:
		x = static_cast<float>(std::uniform_int_distribution<int>{static_cast<int>(-boxsize / 2), static_cast<int>(boxsize / 2)}(prng));
		y = static_cast<float>(std::uniform_int_distribution<int>{static_cast<int>(boxsize / 2), static_cast<int>(3 * boxsize / 2)}(prng));
		break;
	case 2:
		x = static_cast<float>(std::uniform_int_distribution<int>{static_cast<int>(boxsize / 2), static_cast<int>(3 * boxsize / 2)}(prng));
		y = static_cast<float>(std::uniform_int_distribution<int>{static_cast<int>(boxsize / 2), static_cast<int>(3 * boxsize / 2)}(prng));
		break;
	case 3:
		x = static_cast<float>(std::uniform_int_distribution<int>{static_cast<int>(boxsize / 2), static_cast<int>(3 * boxsize / 2)}(prng));
		y = static_cast<float>(std::uniform_int_distribution<int>{static_cast<int>(-boxsize / 2), static_cast<int>(boxsize / 2)}(prng));
		break;
	default:
		x = 0.0f;
		y = 0.0f;
		break;
	}

	return physx::PxVec3(x, y, z);

	

}