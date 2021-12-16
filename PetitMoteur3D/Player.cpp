#include "stdafx.h"
#include "Player.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"

//constructeur du joueur appelant également le constructeur du parent pour dessiner le modele
Player::Player(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, physx::PxVec3 pos)
	: MovingObject(nomfichier, _pDispositif, scale)
{
	//setTesselation(true);
	//création de la hitbox du joueur sous forme de boite et adaptée a la scale
	body = PhysXManager::get().createDynamic(PxTransform(pos), PxBoxGeometry(0.8f *scale, 0.2f * scale, 0.8f *scale),
											PxVec3(0, 0, 0), 0);

	PhysXManager::get().setupFiltering(body, FilterGroup::ePlayer, FilterGroup::ePortal);
	/*contraintes physiques sur le joueur pour avoir un peu de drift et ne pas avoir de mouvement infini
	l'angular damping n'est pas infini pour permettre de tourner dans tous les sens lors d'une collision
	sans non plus tourner a l'infini
	*/
	body->setLinearDamping(0.4f);
	body->setAngularDamping(0.5f);


	//vitesse influant sur les mouvements du joueur
	speed = 30.0f;
}

//fonction d'animation du player
void Player::Anime(float tempEcoule)
{
	//récupération du gestionnaire de saisie afin d'appliquer les mouvements du joueur
	PM3D::CMoteurWindows& rMoteur = PM3D::CMoteurWindows::GetInstance();
	const PM3D::CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();

	// Vérifier l’état des touches pour appliquer les mouvements
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_A))
	{
		rotateLeft();
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_D))
	{
		rotateRight();
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_W))
	{
		moveFoward();
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_S))
	{
		moveBackwards();
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_LSHIFT))
	{
		moveDown();
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_SPACE))
	{
		moveUp();
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_F2) && hasBeenEnoughTimeSinceLastCameraSwitch())
	{
		// Toggle la cam 1ere personne
		camera->toggleFirstPerson();
	}

	//update la vue et la position de la cam en fonction des mouvements effectués plus tot
	updateCam();
	MovingObject::Anime(tempEcoule);

	// TODO Tourner le joueur en fonction du vecteur up de la camera

	const PxVec3 pos = body->getGlobalPose().p;
	const XMFLOAT3 posF3(pos.x, pos.y, pos.z);
	const XMVECTOR posVec = XMLoadFloat3(&posF3);

	const PxQuat pquat = body->getGlobalPose().q;

	const auto camUp = camera->up;
	const PxVec3 vecDir = getDir();
	const float angleY = -atan2(vecDir.z, vecDir.x) - XM_PIDIV2;
	const PxQuat quatY = PxQuat(angleY, { 0.0f, 1.0f, 0.0f });
	const float angleZ = -atan2(vecDir.y, 1.0f);
	const PxQuat quatZ = PxQuat(angleZ, { -1.0f, 0.0f, 0.0f });
	const PxQuat quat = (quatY * quatZ);
	PxQuat temp = pquat;

	if (pquat.w < quat.w - offset) temp.w = pquat.w + offset;
	else if (pquat.w > quat.w + offset) temp.w = pquat.w - offset;

	if (pquat.x < quat.x - offset) temp.x = pquat.x + offset;
	else if (pquat.x > quat.x + offset) temp.x = pquat.x - offset;

	if (pquat.y < quat.y - offset) temp.y = pquat.y + offset;
	else if (pquat.y > quat.y + offset) temp.y = pquat.y - offset;

	if (pquat.z < quat.z - offset) temp.z = pquat.z + offset;
	else if (pquat.z > quat.z + offset) temp.z = pquat.z - offset;

	const XMFLOAT4 quatF3(temp.x, temp.y, temp.z, temp.w);
	const XMVECTOR quatVec = XMLoadFloat4(&quatF3);

	body->setGlobalPose(PxTransform(pos, temp));

	setMatWorld(XMMatrixScaling(scale, scale, scale) * XMMatrixRotationQuaternion(quatVec) * XMMatrixTranslationFromVector(posVec));
}

//avancer
void Player::moveFoward() 
{
	mDir.normalize();
	body->addForce(mDir * speed, PxForceMode::eACCELERATION);
}

//reculer
void Player::moveBackwards() 
{
	mDir.normalize();
	body->addForce(mDir * -speed, PxForceMode::eACCELERATION);
}

//roulis x droite
void Player::rotateRight() 
{
	const PxQuat quat(0.05f, mDir);
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));
}

//retourner la caméra liée au joueur
PM3D::CCamera* Player::getCam()
{
	return camera;
}

//roulis x gauche
void Player::rotateLeft() 
{
	const PxQuat quat(-0.05f, mDir);
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));
}

//monter
void Player::moveUp()
{
	mUp.normalize();
	body->addForce(mUp * 3.0f, PxForceMode::eIMPULSE);
}

//descendre
void Player::moveDown()
{
	mUp.normalize();
	body->addForce(mUp * -3.0f, PxForceMode::eIMPULSE);
}

//assignation de la cam
void Player::setCam(PM3D::CCamera* cam)
{
	camera = cam;
}

//update de la cam en fonction du player et inversement
void Player::updateCam() 
{
	mDir.normalize();
	const PxVec3 bodyPos = body->getGlobalPose().p;
	const XMFLOAT3 camPos(bodyPos.x, bodyPos.y, bodyPos.z);
	const XMFLOAT3 direction(mDir.x, mDir.y, mDir.z);
	camera->updateCam(camPos, direction);

	XMFLOAT3 dir, up;
	XMStoreFloat3(&dir, camera->forward);
	XMStoreFloat3(&up, camera->up);
	mDir = PxVec3(dir.x, dir.y, dir.z);
	mUp = PxVec3(up.x, up.y, up.z);
}

PxVec3 Player::getDir() {
	return mDir.getNormalized();
}

PxVec3 Player::getCameraDir() {
	const XMVECTOR cameraDirection = camera->getDirection();
	const float x = cameraDirection.vector4_f32[0];
	const float y = cameraDirection.vector4_f32[1];
	const float z = cameraDirection.vector4_f32[2];

	return PxVec3{ x,y,z };
}

bool Player::hasBeenEnoughTimeSinceLastCameraSwitch()
{
	if (horloge.GetTimeBetweenCounts(lastPaused, horloge.GetTimeCount()) >= 1)
	{
		lastPaused = horloge.GetTimeCount();
		return true;
	}
	return false;
}
