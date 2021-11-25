#pragma once
#include "objet3d.h"
#include "Ship.h"
#include "PhysXManager.h"



class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

class Asteroid : public Ship {

public:
	Asteroid(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, PxVec3 pos, float boxSize, float scale = 1);
	virtual void Anime(float tempEcoule) override;

private:
	float boxSize;
};



