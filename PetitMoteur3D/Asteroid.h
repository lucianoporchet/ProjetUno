#pragma once
#include "objet3d.h"
#include "MovingObject.h"
#include "PhysXManager.h"



class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

class Asteroid : public MovingObject {

public:
	Asteroid(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, PxVec3 pos, float boxSize, float scale = 1);

	//update la position et rotation de l'objet en fonction des calculs de physX
	virtual void Anime(float tempEcoule) override;

private:
	//taille de la map
	float boxSize;
};



