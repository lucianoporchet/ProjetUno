#pragma once
#include "MovingObject.h"

class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

class NPC : public MovingObject
{
public:
	NPC(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale = 1);
	virtual void Anime(float tempEcoule) override;

	void move();

};

