#pragma once
#include "objet3d.h"
#include "Ship.h"
#include "PhysXManager.h"
#include "Camera.h"


class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

class Player : public Ship {

public:
	Player(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale = 1);
	virtual void Anime(float tempEcoule) override;

	void moveFoward();
	void moveBackwards();
	void rotateLeft();
	void moveUp();
	void moveDown();
	void rotateRight();

	void setCam(PM3D::CCamera* cam);
	void updateCam();

private:

	
	float			angleRotation = PxPi/2;
	PM3D::CCamera*	camera;

};



