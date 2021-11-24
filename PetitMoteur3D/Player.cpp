#include "stdafx.h"
#include "Player.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"


Player::Player(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale)
	: Ship(nomfichier, _pDispositif, scale)
{
	body = PhysXManager::get().createDynamic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)), PxBoxGeometry(0.8f *scale, 0.2f * scale, 0.8f *scale),
											PxVec3(0, 0, 0), PhysXManager::FilterGroup::ePlayer);
	body->setLinearDamping(0.5f);
	body->setAngularDamping(0.5f);
	PhysXManager::get().addToScene(body);
	speed = 30.0f;
}

void Player::Anime(float tempEcoule)
{
	PM3D::CMoteurWindows& rMoteur = PM3D::CMoteurWindows::GetInstance();
	const PM3D::CDIManipulateur& rGestionnaireDeSaisie =
		rMoteur.GetGestionnaireDeSaisie();

	/*PxQuat quat(0.02f, mUp);
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));*/
	// Vérifier l’état de la touche gauche
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_A))
	{
		roateLeft();
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_D))
	{
		roateRight();
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
	const PxVec3 posCam = body->getGlobalPose().p - (mDir * 10);
	// if we use this line, we gotta angle down a bit
	//const PxVec3 posCam = body->getGlobalPose().p - (mDir * 10) + PxVec3{ 0, 3, 0 };
	
	const XMFLOAT3 camPos(posCam.x, posCam.y, posCam.z);
	//const XMFLOAT3 pPos(body->getGlobalPose().p.x, body->getGlobalPose().p.y, body->getGlobalPose().p.z);
	//const XMFLOAT3 camDir(mDir.x, mDir.y, mDir.z);
	camera->updateCam(camPos);

	XMFLOAT3 dir, up;
	XMStoreFloat3(&dir, camera->foward);
	XMStoreFloat3(&up, camera->up);
	mDir = PxVec3(dir.x, dir.y, dir.z);
	mUp = PxVec3(up.x, up.y, up.z);
	
	
	/*PxVec3 dirc(1.0, 0.0, 0.0);
	PxVec3 turnc = PxVec3(0.0f, 1.0f, 0.0f);
	PxQuat anglerotc(angleRotation, turnc);
	dirc = anglerotc.rotate(dirc);
	PxVec3 pov(-13.0f, 0.0f, 0.0f);
	pov = anglerotc.rotate(pov);
	const XMFLOAT3 dirc3(dirc.x, dirc.y, dirc.z);
	PxVec3 position = body->getGlobalPose().p;
	const XMFLOAT3 pov3(pov.x + position.x, pov.y + position.y, pov.z + position.z);
	camera->updateCam(pov3, dirc3);*/



	Ship::Anime(tempEcoule);
}
	
void Player::moveFoward() {

	mDir.normalize();
	body->addForce(mDir * speed, PxForceMode::eACCELERATION);
	//const float tilt = body->getGlobalPose().q.x;
	/*if (tilt >= -0.1)
	{
		body->setAngularVelocity(PxVec3(-0.2f, 0.0f, 0.0f));
	}
	else
	{
		body->setAngularVelocity(PxVec3(0.0f));
	}*/
}
void Player::moveBackwards() {

	mDir.normalize();
	body->addForce(mDir * -speed, PxForceMode::eACCELERATION);
	/*const float tilt = body->getGlobalPose().q.x;
	if (tilt <= 0.1)
	{
		body->setAngularVelocity(PxVec3(0.2f, 0.0f, 0.0f));
	}
	else
	{
		body->setAngularVelocity(PxVec3(0.0f));
	}*/
}

void Player::roateRight() {

	/*PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();
	viewY.normalize();
	body->addForce(viewY * (speed/2), PxForceMode::eIMPULSE);
	const float tilt = body->getGlobalPose().q.x;
	if (tilt <= 0.15)
	{
		body->setAngularVelocity(PxVec3(0.05f, 0.0f, 0.0f));
	}
	else
	{
		body->setAngularVelocity(PxVec3(0.0f));
	}*/
	PxQuat quat(0.05f, mDir);
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));
	
	
	/*mDir = quat.rotate(mDir);
	mDir.normalize();
	*/

	/*PxQuat rotation(-PxHalfPi / 16.0f, { 0, 1, 0 });

	PxQuat newRotation = body->getGlobalPose().q * rotation.getNormalized();

	body->setGlobalPose(PxTransform(body->getGlobalPose().p, newRotation.getNormalized()));

	mDir = rotation.rotate(mDir).getNormalized();*/
	
	/*PxVec3 pos = body->getGlobalPose().p;
	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();
	PxMat33 rm(mDir.cross(viewY), viewY, -mDir);*/

	//body->setGlobalPose(PxTransform(pos, PxQuat(rm)));
	
}
void Player::roateLeft() {

	/*PxVec3 pos = body->getGlobalPose().p;
	float toRot = PxHalfPi / 180;
	angleRotation += toRot;
	
	PxQuat angle(angleRotation, PxVec3(0.0f,1.0f,0.0f));
	mDir = angle.rotate(PxVec3(1.0f, 0.0f, 0.0f));
	body->setGlobalPose(PxTransform(pos, angle));
	*/
	
	
	
	/*PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();
	viewY.normalize();
	body->addForce(viewY * -(speed/2), PxForceMode::eIMPULSE);
	const float tilt = body->getGlobalPose().q.x;
	if (tilt <= 0.15)
	{
		body->setAngularVelocity(PxVec3(0.05f, 0.0f, 0.0f));
	}
	else
	{
		body->setAngularVelocity(PxVec3(0.0f));
	}*/

	PxQuat quat(-0.05f, mDir);
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));
	/*mDir = quat.rotate(mDir);
	mDir.normalize();*/

	/*PxQuat rotation(PxHalfPi / 16.0f, { 0, 1, 0 });

	PxQuat newRotation = body->getGlobalPose().q * rotation.getNormalized();

	body->setGlobalPose(PxTransform(body->getGlobalPose().p, newRotation.getNormalized()));

	mDir = rotation.rotate(mDir).getNormalized();*/
	/*PxVec3 pos = body->getGlobalPose().p;
	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();
	PxMat33 rm(mDir.cross(viewY), viewY, -mDir);*/

	//body->setGlobalPose(PxTransform(pos, PxQuat(rm)));
	
}

void Player::moveUp()
{
	//PxVec3 viewZ = mDir.cross(PxVec3(1, 0, 0)).getNormalized();
	mUp.normalize();
	body->addForce(mUp * (speed/10), PxForceMode::eIMPULSE);
}

void Player::moveDown()
{
	mUp.normalize();
	body->addForce(mUp * -(speed /10), PxForceMode::eIMPULSE);
}


void Player::setCam(PM3D::CCamera* cam)
{
	camera = cam;
}
