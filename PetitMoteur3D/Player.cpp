#include "stdafx.h"
#include "Player.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"


Player::Player(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale)
	: MovingObject(nomfichier, _pDispositif, scale)
{
	body = PhysXManager::get().createDynamic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)), PxBoxGeometry(0.8f *scale, 0.2f * scale, 0.8f *scale),
											PxVec3(0, 0, 0), PhysXManager::FilterGroup::ePlayer);
	body->setLinearDamping(0.4f);
	body->setAngularDamping(0.5f);
	PhysXManager::get().addToScene(body);
	speed = 30.0f;
	
}

void Player::Anime(float tempEcoule)
{
	PM3D::CMoteurWindows& rMoteur = PM3D::CMoteurWindows::GetInstance();
	const PM3D::CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();

	/*PxQuat quat(0.02f, mUp);
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));*/
	// Vérifier l’état de la touche gauche
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
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_F2))
	{
		// breakpoint
		// change camera mode
	}

	updateCam();
	MovingObject::Anime(tempEcoule);
}
	
void Player::moveFoward() 
{
	mDir.normalize();
	body->addForce(mDir * speed, PxForceMode::eACCELERATION);
}
void Player::moveBackwards() 
{
	mDir.normalize();
	body->addForce(mDir * -speed, PxForceMode::eACCELERATION);
}

void Player::rotateRight() 
{
	const PxQuat quat(0.05f, mDir);
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));
}
void Player::rotateLeft() 
{
	const PxQuat quat(-0.05f, mDir);
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));
}

void Player::moveUp()
{
	/*mUp.normalize();
	body->addForce(mUp * (speed/10), PxForceMode::eIMPULSE);*/
	const PxQuat quat(0.05f, mDir.cross(mUp));
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));
}

void Player::moveDown()
{
	/*mUp.normalize();
	body->addForce(mUp * -(speed /10), PxForceMode::eIMPULSE);*/
	const PxQuat quat(-0.05f, mDir.cross(mUp));
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));
}


void Player::setCam(PM3D::CCamera* cam)
{
	camera = cam;
}

void Player::updateCam() 
{
	mDir.normalize();
	const PxVec3 posCam = body->getGlobalPose().p - (mDir * 10);
	const XMFLOAT3 camPos(posCam.x, posCam.y, posCam.z);
	camera->updateCam(camPos);

	XMFLOAT3 dir, up;
	XMStoreFloat3(&dir, camera->forward);
	XMStoreFloat3(&up, camera->up);
	mDir = PxVec3(dir.x, dir.y, dir.z);
	mUp = PxVec3(up.x, up.y, up.z);
}
