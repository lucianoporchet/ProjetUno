#include "stdafx.h"
#include "Planet.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"



Planet::Planet(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, PxVec3 pos)
	: MovingObject(nomfichier, _pDispositif, scale)
{
	//cree le rigid body de l'objet dans physX avec, pour le moment un sphere collider
	body = PhysXManager::get().createDynamic(PxTransform(pos), PxSphereGeometry(scale), PxVec3(0, 0, 0), PhysXManager::FilterGroup::eObstacle);


	body->setAngularDamping(0.f); //enleve l'angular damping pour que les planetes tournent sans s'arreter
	body->setLinearDamping(0.5f); //ajoute du linear damping pour qu'en cas de grosse collision la planete de bouge pas a l'infini
	body->setMass(scale * 10000); //ajoute une masse proportionnelle a la taille


	//choisi un sens de rotation pour la Planete, aleatoirement entre 4 sens de rotation definis
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


}

void Planet::Anime(float tempEcoule)
{
	//fait appel a l'Anime du parent
	MovingObject::Anime(tempEcoule);
}
