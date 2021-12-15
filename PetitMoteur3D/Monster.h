#pragma once
#include "objet3d.h"
#include "ObjetMesh.h"
#include "MovingObject.h"
#include "PhysXManager.h"
#include "chrono"

using namespace std::chrono;

class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

//class Monstre
class Monster : public MovingObject
{
private:
	steady_clock::time_point timeOfLastAttack = high_resolution_clock().now();
	steady_clock::duration attackCooldown = seconds(8);

public:
	Monster(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale = 1, PxVec3 pos = PxVec3(0.0f),int scene = 0);

	bool readyToAttack();
	//update la position et rotation de l'objet en fonction des calculs de physX
	virtual void Anime(float tempEcoule) override;
};

