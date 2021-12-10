#pragma once
#include "objet3d.h"
#include "ObjetMesh.h"
#include "PhysXManager.h"



class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

//classe pour tous les objets qui bougent avec de la physique
class Portal : public PM3D::CObjetMesh {

public:
	Portal(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, PxVec3 pos, int scene);

	//update la position et rotation de l'objet en fonction des calculs de physX
	virtual void Anime(float tempEcoule) override;


	
	PxRigidDynamic* body;
protected:

	float	scale;

};



