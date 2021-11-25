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
	body->addTorque(PxVec3(scale * 10000, scale * 10000, 0), PxForceMode::eIMPULSE);
	body->setLinearDamping(0.5f);
	body->setMass(scale * 10000);

	PhysXManager::get().addToScene(body);
}

void Planet::Anime(float tempEcoule)
{
	MovingObject::Anime(tempEcoule);
}
