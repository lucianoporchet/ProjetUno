#include "stdafx.h"
#include "Player.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"


Player::Player(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale)
	: CObjetMesh(nomfichier, _pDispositif, scale), scale{scale}
{
	body = PhysXManager::get().createDynamic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)), PxBoxGeometry(0.8f *scale, 0.2f * scale, 0.8f *scale), PxVec3(0, 0, 0), PhysXManager::FilterGroup::ePlayer);
	/*body->addTorque(PxVec3(100000, 1000000000, 0), PxForceMode::eIMPULSE);
	body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);*/
	body->setLinearDamping(0.5f);
	mDir = PxVec3(0, 0, -1);
	mMouseX = 0;
	mMouseY = 0;
	PhysXManager::get().addToScene(body);
}

void Player::Anime(float tempEcoule)
{
	PM3D::CMoteurWindows& rMoteur = PM3D::CMoteurWindows::GetInstance();
	const PM3D::CDIManipulateur& rGestionnaireDeSaisie =
		rMoteur.GetGestionnaireDeSaisie();
	// Vérifier l’état de la touche gauche
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_A))
	{
		moveLeft();
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_D))
	{
		moveRight();
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
	//au click gauche 
	if (rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] != 0)
	{
		
	}

	mDir.normalize();
	const PxVec3 posCam = body->getGlobalPose().p - mDir * 10;
	const XMFLOAT3 camPos(posCam.x, posCam.y, posCam.z);
	camera->updateCam(camPos);


	const PxVec3 pos = body->getGlobalPose().p;
	const XMFLOAT3 posF3(pos.x, pos.y, pos.z);
	const XMVECTOR posVec = XMLoadFloat3(&posF3);

	const PxQuat quat = body->getGlobalPose().q;
	const XMFLOAT4 quatF3(quat.x, quat.y, quat.z, quat.w);
	const XMVECTOR quatVec = XMLoadFloat4(&quatF3);

	setMatWorld(XMMatrixScaling(scale, scale, scale) * XMMatrixRotationQuaternion(quatVec) * XMMatrixTranslationFromVector(posVec));
}
	
void Player::moveFoward() {

	mDir.normalize();
	body->addForce(mDir * speed, PxForceMode::eACCELERATION);
	const float angle = body->getGlobalPose().q.getAngle();
	const float tilt = body->getGlobalPose().q.x;
	if (tilt >= -0.1)
	{
		body->setAngularVelocity(PxVec3(-0.2f, 0.0f, 0.0f));
	}
	else
	{
		body->setAngularVelocity(PxVec3(0.0f));
	}
}
void Player::moveBackwards() {

	mDir.normalize();
	body->addForce(mDir * -speed, PxForceMode::eACCELERATION);
	const float angle = body->getGlobalPose().q.getAngle();
	const float tilt = body->getGlobalPose().q.x;
	if (tilt <= 0.1)
	{
		body->setAngularVelocity(PxVec3(0.2f, 0.0f, 0.0f));
	}
	else
	{
		body->setAngularVelocity(PxVec3(0.0f));
	}
}

void Player::moveRight() {

	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();
	viewY.normalize();
	body->addForce(viewY * speed, PxForceMode::eIMPULSE);
	const float angle = body->getGlobalPose().q.getAngle();
	
	body->setAngularVelocity(PxVec3(0.1f, 0.0f, 0.0f)/(angle+0.05f));
	/*if (angle <= 0.45)
	{
		body->setAngularVelocity(PxVec3(0.1f, 0.0f, 0.0f));
	}
	else
	{
		body->setAngularVelocity(PxVec3(0.01f));
	}*/
	
}
void Player::moveLeft() {

	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();
	viewY.normalize();
	body->addForce(viewY * -speed, PxForceMode::eIMPULSE);
	const float angle = body->getGlobalPose().q.getAngle();

	body->setAngularVelocity(PxVec3(0.1f, 0.0f, 0.0f) / (angle + 0.05f));
	
}

void Player::moveUp()
{
	PxVec3 viewZ = mDir.cross(PxVec3(1, 0, 0)).getNormalized();
	viewZ.normalize();
	body->addForce(viewZ * -(speed/3), PxForceMode::eIMPULSE);
}
void Player::moveDown()
{
	PxVec3 viewZ = mDir.cross(PxVec3(1, 0, 0)).getNormalized();
	viewZ.normalize();
	body->addForce(viewZ * (speed/3), PxForceMode::eIMPULSE);
}


void Player::setCam(PM3D::CCamera* cam)
{
	camera = cam;
}
