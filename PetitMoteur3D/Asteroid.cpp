#include "stdafx.h"
#include "Asteroid.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"


Asteroid::Asteroid(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, PxVec3 pos)
	: MovingObject(nomfichier, _pDispositif, scale)
{
	//on donne une vitesse aleatoire aux asteroides entre 150 et 500
	float rSpeed = static_cast<float>(RandomGenerator::get().next(150, 500));

	//cree le rigid body de l'objet dans physX avec, pour le moment un capsule collider (donc collisions pas parfaites)
	body = PhysXManager::get().createDynamic(PxTransform(PxVec3(pos)), PxCapsuleGeometry(scale,scale*1.3f), PxVec3(0, 0, 0), PhysXManager::FilterGroup::eObstacle);

	//ajoute une rotation aleatoire, une force en direction du centre de la carte (la direction sera changée plus tard), et une masse proportionnelle a la taille
	body->addTorque(RandomGenerator::get().randomVec3(150, 300) * 1000.0f, PxForceMode::eIMPULSE);
	body->addForce(PxVec3(-pos) * rSpeed, PxForceMode::eIMPULSE);
	body->setMass(scale * 10);

}

void Asteroid::Anime(float tempEcoule)
{
	
	//TO DO: code pour gerer les asteroides qui arrivent en bord de map
	
	//fait appel a l'Anime du parent
	MovingObject::Anime(tempEcoule);
}
