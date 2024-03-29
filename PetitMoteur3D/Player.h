#pragma once
#include "objet3d.h"
#include "MovingObject.h"
#include "PhysXManager.h"
#include "Camera.h"


class PM3D::IChargeur;
class PM3D::CDispositifD3D11;

//classe représentant le joueur
class Player : public MovingObject {

public:
	Player(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale = 1, physx::PxVec3 pos = physx::PxVec3(0.0f));
	virtual void Anime(float tempEcoule) override;

	/*
	Mouvements disponibles pour le joueur
	*/
	void moveFoward();
	void moveBackwards();
	void rotateLeft();
	void moveUp();
	void moveDown();
	void rotateRight();
	PM3D::CCamera* getCam();
	bool hasBeenEnoughTimeSinceLastCameraSwitch();

	/*
	Ajouter la cam liée au joueur actuellement et update sa position et direction
	*/
	void setCam(PM3D::CCamera* cam);
	void updateCam();
	physx::PxVec3 getDir();
	physx::PxVec3 getCameraDir();
	
private:
	
	const float		offset{ 0.005f };

	PM3D::CCamera*	camera;
	PM3D::Horloge horloge;
	int64_t lastPaused = 0;

};



