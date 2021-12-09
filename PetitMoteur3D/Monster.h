#pragma once
#include "objet3d.h"
#include "ObjetMesh.h"
#include "MovingObject.h"
#include "PhysXManager.h"

class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

//class Monstre
class Monster : public MovingObject
{
public:
	Monster(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale = 1, PxVec3 pos = PxVec3(0.0f));

	//update la position et rotation de l'objet en fonction des calculs de physX
	virtual void Anime(float tempEcoule) override;
	//TO DO
};

