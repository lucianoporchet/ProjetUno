#include "stdafx.h"
#include "Obstacle.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"


Obstacle::Obstacle(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, bool planet)
	: CObjetMesh(nomfichier, _pDispositif, scale), scale{ scale }, planet{ planet }
{
	const float x = static_cast<float>(RandomGenerator::get().next(-25, 25));
	const float y = static_cast<float>(RandomGenerator::get().next(-25, 25));
	const float z = static_cast<float>(RandomGenerator::get().next(-200, -100));
	if (planet) {
		body = PhysXManager::get().createDynamic(PxTransform(PxVec3(x, y, z)), PxSphereGeometry(scale), PxVec3(0, 0, 0), PhysXManager::FilterGroup::eObstacle);
		body->addTorque(PxVec3(scale * 1000, scale * 1000, 0), PxForceMode::eIMPULSE);
		body->setLinearDamping(0.5f);
	}
	else {
		body = PhysXManager::get().createDynamic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)), PxCapsuleGeometry(scale, scale), PxVec3(0, 0, 0), PhysXManager::FilterGroup::eObstacle);
		body->addTorque(PxVec3(scale, scale, 0), PxForceMode::eIMPULSE);
		body->addForce(RandomGenerator::get().randomVec3(static_cast<int>(-scale) * 10, static_cast<int>(scale) * 10), PxForceMode::eIMPULSE);
	}	
	body->setMass(scale);
	PhysXManager::get().addToScene(body);
}

void Obstacle::Anime(float tempEcoule)
{
	PM3D::CMoteurWindows& rMoteur = PM3D::CMoteurWindows::GetInstance();
	const PM3D::CDIManipulateur& rGestionnaireDeSaisie =
		rMoteur.GetGestionnaireDeSaisie();
	
	const PxVec3 pos = body->getGlobalPose().p;
	const XMFLOAT3 posF3(pos.x, pos.y, pos.z);
	const XMVECTOR posVec = XMLoadFloat3(&posF3);

	const PxQuat quat = body->getGlobalPose().q;
	const XMFLOAT4 quatF3(quat.x, quat.y, quat.z, quat.w);
	const XMVECTOR quatVec = XMLoadFloat4(&quatF3);

	setMatWorld(XMMatrixScaling(scale, scale, scale) * XMMatrixRotationQuaternion(quatVec) * XMMatrixTranslationFromVector(posVec));
}
