#pragma once
#include "objet3d.h"
#include "ObjetMesh.h"
#include "PhysXManager.h"



class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

//classe pour tous les objets qui bougent avec de la physique
class TunnelComponent : public PM3D::CObjetMesh {

public:
	TunnelComponent(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, PxVec3 scale, PxVec3 pos, PxQuat quat);

	//update la position et rotation de l'objet en fonction des calculs de physX
	virtual void Anime(float tempEcoule) override;



	PxRigidStatic* body;
protected:

	int scene = 1;

};



