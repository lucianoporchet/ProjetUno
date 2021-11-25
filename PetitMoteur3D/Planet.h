#pragma once
#include "objet3d.h"
#include "MovingObject.h"
#include "PhysXManager.h"



class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

class Planet : public MovingObject {

public:
	Planet(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, PxVec3 pos, float scale = 1);
	virtual void Anime(float tempEcoule) override;
};



