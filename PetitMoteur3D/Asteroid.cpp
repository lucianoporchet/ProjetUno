#include "stdafx.h"
#include "Asteroid.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"


Asteroid::Asteroid(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, PxVec3 pos, int scene)
	: MovingObject(nomfichier, _pDispositif, scale), scene{scene}
{
	//on donne une vitesse aleatoire aux asteroides
	speed = static_cast<float>(RandomGenerator::get().next(500, 10000));
	
	mDir = (RandomGenerator::get().randomPosInZone(scene) - pos) * speed;
	//cree le rigid body de l'objet dans physX avec, pour le moment un capsule collider (donc collisions pas parfaites)
	body = PhysXManager::get().createDynamic(PxTransform(PxVec3(pos)), PxCapsuleGeometry(scale,scale*1.3f), PxVec3(0, 0, 0), scene);

	PhysXManager::get().addForcesAsteroid(scale, body, mDir);


}

void Asteroid::Anime(float tempEcoule)
{
	MovingObject::Anime(tempEcoule);
}
