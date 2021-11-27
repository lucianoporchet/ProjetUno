#include "stdafx.h"
#include "Player.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"

//constructeur du joueur appelant également le constructeur du parent pour dessiner le modele
Player::Player(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale)
	: MovingObject(nomfichier, _pDispositif, scale)
{
	//setTesselation(true);
	//création de la hitbox du joueur sous forme de boite et adaptée a la scale
	body = PhysXManager::get().createDynamic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)), PxBoxGeometry(0.8f *scale, 0.2f * scale, 0.8f *scale),
											PxVec3(0, 0, 0), PhysXManager::FilterGroup::ePlayer);

	/*contraintes physiques sur le joueur pour avoir un peu de drift et ne pas avoir de mouvement infini
	l'angular damping n'est pas infini pour permettre de tourner dans tous les sens lors d'une collision
	sans non plus tourner a l'infini
	*/
	body->setLinearDamping(0.4f);
	body->setAngularDamping(0.5f);

	//ajout de l'acteur physique a la scene physique
	PhysXManager::get().addToScene(body);
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
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_F2))
	{
		// breakpoint
		// Toggle la cam 1ere personne
		camera->toggleFirstPerson();
	}

	//update la vue et la position de la cam en fonction des mouvements effectués plus tot
	updateCam();
	MovingObject::Anime(tempEcoule);
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

//roulis x gauche
void Player::rotateLeft() 
{
	const PxQuat quat(-0.05f, mDir);
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));
}

//roulis y gauche
void Player::moveUp()
{
	/*mUp.normalize();
	body->addForce(mUp * (speed/10), PxForceMode::eIMPULSE);*/
	const PxQuat quat(0.05f, mDir.cross(mUp));
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));
}

//roulis y droite
void Player::moveDown()
{
	/*mUp.normalize();
	body->addForce(mUp * -(speed /10), PxForceMode::eIMPULSE);*/
	const PxQuat quat(-0.05f, mDir.cross(mUp));
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, body->getGlobalPose().q * quat.getNormalized()));
}

//assignation de la cam
void Player::setCam(PM3D::CCamera* cam)
{
	camera = cam;
}

//update de la cam en fonction du player et inversement
void Player::updateCam() 
{
	//mDir.normalize();
	//const PxVec3 posCam = body->getGlobalPose().p - (mDir * 10);
	//const XMFLOAT3 camPos(posCam.x, posCam.y, posCam.z);
	//camera->updateCam(camPos, body->GetGlo);
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
