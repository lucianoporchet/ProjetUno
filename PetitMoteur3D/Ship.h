#pragma once
#include "objet3d.h"
#include "ObjetMesh.h"
#include "PhysXManager.h"



class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

class Ship : public PM3D::CObjetMesh {

public:
	Ship(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale = 1);
	virtual void Anime(float tempEcoule) override;

protected:

	PxRigidDynamic* body;
	PxVec3			mDir;
	PxVec3			mUp;
	float			speed = 3;
	float			scale;
	float			angleRotation = PxPi / 2;
};



