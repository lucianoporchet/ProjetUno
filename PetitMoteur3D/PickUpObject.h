#pragma once
#include "objet3d.h"
#include "ObjetMesh.h"
#include "PhysXManager.h"

enum class PickUpObjectType 
{
	GreenKey,
	BlueKey,
	RedKey,
	SpeedBuff
};

struct PickUpObjectPlacementInfo 
{
	PxVec3 pos;
	PickUpObjectType objectType;
	int zoneNumber;
};

class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

class PickUpObject : public PM3D::CObjetMesh
{
public:
	PickUpObject(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif,PickUpObjectType objType, float scale, PxVec3 pos, int scene);

	//update la position et rotation de l'objet en fonction des calculs de physX
	virtual void Anime(float tempEcoule) override;
	PickUpObjectType getType();


	PxRigidDynamic* body;
protected:
	PickUpObjectType objectType;
	float	scale;
};

