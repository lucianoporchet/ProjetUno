#pragma once
#include "objet3d.h"
#include "ObjetMesh.h"
#include "PhysXManager.h"



class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

//classe pour tous les objets qui bougent avec de la physique
class MovingObject : public PM3D::CObjetMesh {

public:
	MovingObject(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale = 1);

	//getters et setters de la vitesse
	float getSpeed();
	void setSpeed(float newSpeed);

	//update la position et rotation de l'objet en fonction des calculs de physX
	virtual void Anime(float tempEcoule) override;

	PxRigidDynamic* body;
protected:

	
	PxVec3			mDir;
	PxVec3			mUp;
	float			speed = 3;
	float			scale;
	float			angleRotation = PxPi / 2;
};



