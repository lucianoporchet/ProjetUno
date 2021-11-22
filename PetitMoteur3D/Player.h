#pragma once
#include "objet3d.h"
#include "ObjetMesh.h"
#include "PhysXManager.h"


class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

class Player : public PM3D::CObjetMesh {

public:
	Player(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale = 1);
	virtual void Anime(float tempEcoule) override;

	void moveFoward();
	void moveBackwards();
	void moveLeft();
	void moveUp();
	void moveDown();
	void moveRight();

private:

	PxRigidDynamic* body;
	int				mMouseX;
	int				mMouseY;
	float			speed = 3;
	PxVec3			mDir;
	float			scale;

};



