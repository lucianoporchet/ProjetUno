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
	body = PhysXManager::get().createDynamic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)), PxSphereGeometry(scale), PxVec3(0, 0, 0), PhysXManager::FilterGroup::ePlayer);
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
	PM3D::CDIManipulateur& rGestionnaireDeSaisie =
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


	PxVec3 pos = body->getGlobalPose().p;
	XMFLOAT3 posF3(pos.x, pos.y, pos.z);
	XMVECTOR posVec = XMLoadFloat3(&posF3);

	PxQuat quat = body->getGlobalPose().q;
	XMFLOAT4 quatF3(quat.x, quat.y, quat.z, quat.w);
	XMVECTOR quatVec = XMLoadFloat4(&quatF3);

	setMatWorld(XMMatrixScaling(scale, scale, scale) * XMMatrixRotationQuaternion(quatVec) * XMMatrixTranslationFromVector(posVec));
}
	
void Player::moveFoward() {

	mDir.normalize();
	body->addForce(mDir * speed, PxForceMode::eACCELERATION);
}
void Player::moveBackwards() {

	mDir.normalize();
	body->addForce(mDir * -speed, PxForceMode::eACCELERATION);
}
void Player::moveRight() {

	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();
	viewY.normalize();
	body->addForce(viewY * speed, PxForceMode::eIMPULSE);
}
void Player::moveLeft() {

	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();
	viewY.normalize();
	body->addForce(viewY * -speed, PxForceMode::eIMPULSE);
}
