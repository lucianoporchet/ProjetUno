#pragma once
#include "objet3d.h"
#include "MovingObject.h"
#include "PhysXManager.h"



class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

class Planet : public MovingObject {

public:
	Planet(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, PxVec3 pos, int scene);

	//update la position et rotation de l'objet en fonction des calculs de physX
	virtual void Anime(float tempEcoule) override;
};



