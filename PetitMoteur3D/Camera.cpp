#include "stdafx.h"
#include "Camera.h"

#include "util.h"
#include "DispositifD3D11.h"


#include "resource.h"
#include "MoteurWindows.h"
using namespace DirectX;

namespace PM3D {
	
	void CCamera::update3RD(XMFLOAT3 camPos, XMFLOAT3 direction) {
		// decalage 3e personne
		camPos.x = camPos.x - direction.x * 10;
		camPos.y = camPos.y - direction.y * 10;
		camPos.z = camPos.z - direction.z * 10;

		XMStoreFloat3(&pos, position);
		XMFLOAT3 dir(camPos.x - pos.x, camPos.y - pos.y, camPos.z - pos.z);
		pos.x = pos.x + dir.x * 0.15f;
		pos.y = pos.y + dir.y * 0.15f;
		pos.z = pos.z + dir.z * 0.15f;
		position = XMLoadFloat3(&pos);


		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();

		if ((0x80)) {
			if (rGestionnaireDeSaisie.EtatSouris().lY != 0 || rGestionnaireDeSaisie.EtatSouris().lX != 0)
			{
				float temp = pitch;
				yaw -= (float)rGestionnaireDeSaisie.EtatSouris().lX;
				pitch -= (float)rGestionnaireDeSaisie.EtatSouris().lY;
				if (pitch < -1600.0f || pitch > 1400.0f) pitch = temp;
				rot = XMFLOAT3(pitch * 0.0009f, yaw * 0.0009f, 0.0f);
				rotation = XMLoadFloat3(&rot);

			}
		}
		if (!GameManager::get().getIsPauseStatus())
		{
			rGestionnaireDeSaisie.setSourisPosition(cursorPosx, cursorPosy);
		}
		
	}

	void CCamera::update1ST(XMFLOAT3 camPos, XMFLOAT3 direction)
	{
		// Pas de decalage 3e personne ici 

		XMStoreFloat3(&pos, position);
		XMFLOAT3 dir(camPos.x - pos.x, camPos.y - pos.y, camPos.z - pos.z);
		// TODO a ameliorer, on clippe dans le modele du joueur. Potentiellement ne pas le rendre?
		pos.x = pos.x + dir.x;
		pos.y = pos.y + dir.y;
		pos.z = pos.z + dir.z;
		position = XMLoadFloat3(&pos);


		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();

		if ((0x80)) {
			if (rGestionnaireDeSaisie.EtatSouris().lY != 0 || rGestionnaireDeSaisie.EtatSouris().lX != 0)
			{
				yaw -= (float)rGestionnaireDeSaisie.EtatSouris().lX;
				pitch -= (float)rGestionnaireDeSaisie.EtatSouris().lY;
				rot = XMFLOAT3(pitch * 0.0009f, yaw * 0.0009f, 0.0f);
				rotation = XMLoadFloat3(&rot);

			}
		}
		if (!GameManager::get().getIsPauseStatus())
		{
			rGestionnaireDeSaisie.setSourisPosition(cursorPosx, cursorPosy);
		}
	}

	void CCamera::updateFREE(XMFLOAT3 camPos)
	{
		// TODO
	}

	CCamera::CCamera(const XMVECTOR& position_in,
		const XMVECTOR& direction_in,
		const XMVECTOR& up_in,
		XMMATRIX* pMatView_in,
		XMMATRIX* pMatProj_in,
		XMMATRIX* pMatViewProj_in)
	{
		Init(position_in, direction_in, up_in, pMatView_in, pMatProj_in, pMatViewProj_in);
	}

	void CCamera::Init(const XMVECTOR& position_in,
		const XMVECTOR& direction_in,
		const XMVECTOR& up_in,
		XMMATRIX* pMatView_in,
		XMMATRIX* pMatProj_in,
		XMMATRIX* pMatViewProj_in)
	{
		pMatView = pMatView_in;
		pMatProj = pMatProj_in;
		pMatViewProj = pMatViewProj_in;
		position = position_in;
		direction = direction_in;
		up = up_in;

		XMStoreFloat3(&pos, position);
		/*XMVector4Normalize(direction);*/

		cursorPosx = CMoteurWindows::GetInstance().pDispositif->GetHauteur() / 2;
		cursorPosy = CMoteurWindows::GetInstance().pDispositif->GetLargeur() / 2;
		

		updateView();
	}


	void CCamera::updateCam(XMFLOAT3 camPos, XMFLOAT3 direction)
	{
		(this->*updateFctPtr)(camPos, direction);
		updateView();
	}

	void CCamera::updateView() {
		const XMMATRIX camRotationM = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		direction = XMVector3TransformCoord(DEFAULT_FOWARD, camRotationM);
		direction += position;

		up = XMVector3TransformCoord(DEFAULT_UP, camRotationM);

		*pMatView = XMMatrixLookAtRH(position,direction,up);
		*pMatViewProj = (*pMatView) * (*pMatProj);

		forward = XMVector3TransformCoord(DEFAULT_FOWARD, camRotationM);
		right = XMVector3TransformCoord(DEFAULT_RIGHT, camRotationM);
	}

	void CCamera::toggleFirstPerson()
	{
		if (updateFctPtr == &CCamera::update3RD)
		{
			updateFctPtr = &CCamera::update1ST;
		}
		else
		{
			updateFctPtr = &CCamera::update3RD;
		}
	}

	bool CCamera::isCamera1st()
	{
		if (updateFctPtr == &CCamera::update3RD)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}
