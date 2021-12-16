#include "stdafx.h"
#include "NPC.h"

NPC::NPC(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, PxVec3 pos, int scene)
	: MovingObject(nomfichier, _pDispositif, scale)
{
	body = PhysXManager::get().createDynamic(PxTransform(pos), PxBoxGeometry(0.8f * scale, 0.2f * scale, 0.8f * scale), PxVec3(0, 0, 0), scene);
	body->setLinearDamping(0.5f);
}

void NPC::Anime(float tempEcoule)
{
	move();
	MovingObject::Anime(tempEcoule);
}

void NPC::move() {

	//TO DO

}