#include "stdafx.h"
#include "Asteroid.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"


Asteroid::Asteroid(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, PxVec3 pos, float boxSize, float scale)
	: Ship(nomfichier, _pDispositif, scale)
{
	float rSpeed = static_cast<float>(RandomGenerator::get().next(150, 500));
	body = PhysXManager::get().createDynamic(PxTransform(PxVec3(pos)), PxSphereGeometry(scale), PxVec3(0, 0, 0), PhysXManager::FilterGroup::eObstacle);
	body->addTorque(RandomGenerator::get().randomVec3(150, 300) * 1000.0f, PxForceMode::eIMPULSE);
	body->addForce(PxVec3(-pos) * rSpeed, PxForceMode::eIMPULSE);
	body->setMass(scale * 10);

	PhysXManager::get().addToScene(body);
}

void Asteroid::Anime(float tempEcoule)
{
	
	
	/*const PxVec3 pos = body->getGlobalPose().p;
	PxVec3 velocity = body->getLinearVelocity();
	int limit = std::abs(static_cast<int>(boxSize / 2));

	if (static_cast<int>(pos.x) > limit || static_cast<int>(pos.y) > limit || static_cast<int>(pos.z) > limit){
		body->setLinearVelocity(-velocity);
	}
	
	const XMFLOAT3 posF3(pos.x, pos.y, pos.z);
	const XMVECTOR posVec = XMLoadFloat3(&posF3);

	const PxQuat quat = body->getGlobalPose().q;
	const XMFLOAT4 quatF3(quat.x, quat.y, quat.z, quat.w);
	const XMVECTOR quatVec = XMLoadFloat4(&quatF3);

	setMatWorld(XMMatrixScaling(scale, scale, scale) * XMMatrixRotationQuaternion(quatVec) * XMMatrixTranslationFromVector(posVec));*/
	
	Ship::Anime(tempEcoule);
}
