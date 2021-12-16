#pragma once
#include "MovingObject.h"

class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

//class pour npc de vaisseaux
class NPC : public MovingObject
{
public:
	NPC(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, PxVec3 pos, int scene);
	virtual void Anime(float tempEcoule) override;

	void move();

};

