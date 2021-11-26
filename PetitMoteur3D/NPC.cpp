#include "stdafx.h"
#include "NPC.h"

NPC::NPC(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale)
	: MovingObject(nomfichier, _pDispositif, scale)
{
	body = PhysXManager::get().createDynamic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)), PxBoxGeometry(0.8f * scale, 0.2f * scale, 0.8f * scale), PxVec3(0, 0, 0), PhysXManager::FilterGroup::eObstacle);
	body->setLinearDamping(0.5f);
	PhysXManager::get().addToScene(body);
}

void NPC::Anime(float tempEcoule)
{
	move();
	MovingObject::Anime(tempEcoule);
}

void NPC::move() {

	//TO DO

}