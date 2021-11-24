#pragma once
#include "objet3d.h"
#include "ObjetMesh.h"
#include "PhysXManager.h"


class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

class Obstacle : public PM3D::CObjetMesh {

public:
	Obstacle(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, PxVec3 pos, float scale = 1, bool planet = false);
	virtual void Anime(float tempEcoule) override;


private:

	PxRigidDynamic* body;
	float			speed = 3;
	float			scale;
	bool			planet = false;
};



