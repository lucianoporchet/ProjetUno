#include "stdafx.h"
#include "Planet.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"



Planet::Planet(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, PxVec3 pos, float scale)
	: MovingObject(nomfichier, _pDispositif, scale)
{
	body = PhysXManager::get().createDynamic(PxTransform(pos), PxSphereGeometry(scale), PxVec3(0, 0, 0), PhysXManager::FilterGroup::eObstacle);
	body->setAngularDamping(0.f);
	body->setLinearDamping(0.5f);
	body->setMass(scale * 10000);
	int rotaAxis = RandomGenerator::get().next(1, 4);
	switch (rotaAxis)
	{
	case 1:
		body->addTorque(PxVec3(static_cast<float>(pow(scale * 2, 4)),0, 0), PxForceMode::eIMPULSE);
		break;
	case 2:
		body->addTorque(PxVec3(0, static_cast<float>(pow(scale * 2, 4)), 0), PxForceMode::eIMPULSE);
		break;
	case 3 :
		body->addTorque(PxVec3(0, 0, static_cast<float>(pow(scale * 2, 4))), PxForceMode::eIMPULSE);
		break;
	case 4:
		body->addTorque(PxVec3(static_cast<float>(pow(scale * 2, 4)), static_cast<float>(pow(scale * 2, 4)), 0), PxForceMode::eIMPULSE);
		break;
	default:
		break;
	}

	

	PhysXManager::get().addToScene(body);
}

void Planet::Anime(float tempEcoule)
{
	MovingObject::Anime(tempEcoule);
}
