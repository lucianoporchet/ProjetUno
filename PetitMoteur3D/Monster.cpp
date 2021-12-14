#include "stdafx.h"
#include "Monster.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"
#include "SceneManager.h"
#include <math.h>
#include <mutex>

//TO DO
static std::mutex monsterMutex;
Monster::Monster(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, PxVec3 pos, int scene) : MovingObject(nomfichier, _pDispositif, scale)
{
	//on donne une vitesse aleatoire au monstre entre 150 et 500
	//float rSpeed = static_cast<float>(RandomGenerator::get().next(15, 50));

	//cree le rigid body de l'objet dans physX avec, pour le moment un capsule collider (donc collisions pas parfaites)
	body = PhysXManager::get().createDynamic(PxTransform(PxVec3(pos)), PxSphereGeometry(scale * 0.5f), PxVec3(0, 0, 0), scene);

	//ajoute une rotation aleatoire, une force en direction du centre de la carte (la direction sera changée plus tard), et une masse proportionnelle a la taille
	//body->addTorque(RandomGenerator::get().randomVec3(150, 300) * 1000.0f, PxForceMode::eIMPULSE);*/
	//PxVec3 posPlayer = SceneManager::get().player->body->getGlobalPose().p;
	//body->addForce(PxVec3(-pos + posPlayer) * rSpeed, PxForceMode::eIMPULSE);
	std::lock_guard<std::mutex> lock(monsterMutex);
	body->setMass(scale * 1000000);
}

bool Monster::readyToAttack()
{
	auto currentTime = high_resolution_clock().now();
	if ((currentTime - timeOfLastAttack) >= attackCooldown) {
		return true;
	}
	return false;
}

void Monster::Anime(float tempEcoule)
{
	const PxVec3 posPlayer = SceneManager::get().player->body->getGlobalPose().p;
	const PxTransform MonsterCurrentInfo = body->getGlobalPose();
	const PxVec3 pos = MonsterCurrentInfo.p;
	const PxVec3 direction = (-pos + posPlayer).getNormalized();
	float angleY = -atan2(direction.z, direction.x) - XM_PI / 2;
	
	const PxQuat quatY = PxQuat(angleY, { 0.0f, 1.0f, 0.0f });
	angleY += XM_PI / 2;

	float angleX = atan2(direction.y, abs(direction.x));
	
	PxQuat quatX = PxQuat(angleX, { sin(angleY), 0.0f, cos(angleY) });
	

	const PxQuat quat = quatX * quatY;
	if (readyToAttack()) {
		speed = 50.0f;
		body->setLinearVelocity(PxVec3((direction).getNormalized()) * speed);
		timeOfLastAttack = high_resolution_clock().now();
	}
	body->setGlobalPose(PxTransform(pos, quat));
	//fait appel a l'Anime du parent
	MovingObject::Anime(tempEcoule);
}
