#include "stdafx.h"
#include "Planet.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"



Planet::Planet(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, PxVec3 pos, int scene)
	: MovingObject(nomfichier, _pDispositif, scale)
{
	//cree le rigid body de l'objet dans physX avec, pour le moment un sphere collider
	body = PhysXManager::get().createDynamic(PxTransform(pos), PxSphereGeometry(scale), PxVec3(0, 0, 0), scene);

	PhysXManager::get().addForcesPlanet(scale, body);
	
}

void Planet::Anime(float tempEcoule)
{
	//fait appel a l'Anime du parent
	MovingObject::Anime(tempEcoule);
}
