#include "stdafx.h"
#include "Camera.h"

#include "util.h"
#include "DispositifD3D11.h"

#include "resource.h"
#include "MoteurWindows.h"
using namespace DirectX;

namespace PM3D {
	
	// TODO On demolit l'encapsulation avec un boulet la, mais j'ai pas trouve mieux
	void Update3RD::update (XMFLOAT3 camPos, CCamera cam) {
		XMStoreFloat3(&cam.pos, cam.position);
		XMFLOAT3 dir(camPos.x - cam.pos.x, camPos.y - cam.pos.y, camPos.z - cam.pos.z);
		cam.pos.x = cam.pos.x + dir.x * 0.15f;
		cam.pos.y = cam.pos.y + dir.y * 0.15f;
		cam.pos.z = cam.pos.z + dir.z * 0.15f;
		cam.position = XMLoadFloat3(&cam.pos);


		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();

		if ((0x80)) {
			if (rGestionnaireDeSaisie.EtatSouris().lY != 0 || rGestionnaireDeSaisie.EtatSouris().lX != 0)
			{
				cam.yaw -= (float)rGestionnaireDeSaisie.EtatSouris().lX;
				cam.pitch -= (float)rGestionnaireDeSaisie.EtatSouris().lY;
				cam.rot = XMFLOAT3(cam.pitch * 0.0009f, cam.yaw * 0.0009f, 0.0f);
				cam.rotation = XMLoadFloat3(&cam.rot);

			}
		}
		if (!rGestionnaireDeSaisie.getIsPauseStatus())
		{
			rGestionnaireDeSaisie.setSourisPosition(cam.cursorPosx, cam.cursorPosy);
		}
	}

	void Update1ST::update(XMFLOAT3 camPos, CCamera cam)
	{
	}

	void UpdateFREE::update(XMFLOAT3 camPos, CCamera cam)
	{
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


	void CCamera::updateCam(XMFLOAT3 camPos)
	{
		//XMStoreFloat3(&pos, position);
		//XMFLOAT3 dir(camPos.x - pos.x, camPos.y - pos.y, camPos.z - pos.z);
		//pos.x = pos.x + dir.x * 0.15f;
		//pos.y = pos.y + dir.y * 0.15f;
		//pos.z = pos.z + dir.z * 0.15f;
		//position = XMLoadFloat3(&pos);
		//

		//CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		//CDIManipulateur& rGestionnaireDeSaisie = rMoteur.GetGestionnaireDeSaisie();

		//if ((0x80)) {
		//	if (rGestionnaireDeSaisie.EtatSouris().lY != 0 || rGestionnaireDeSaisie.EtatSouris().lX != 0)
		//	{
		//		yaw -= (float)rGestionnaireDeSaisie.EtatSouris().lX;
		//		pitch -= (float)rGestionnaireDeSaisie.EtatSouris().lY;
		//		rot = XMFLOAT3(pitch * 0.0009f, yaw * 0.0009f, 0.0f);
		//		rotation = XMLoadFloat3(&rot);
		//		
		//	}
		//}
		//if (!rGestionnaireDeSaisie.getIsPauseStatus())
		//{
		//	rGestionnaireDeSaisie.setSourisPosition(cursorPosx, cursorPosy);
		//}
		
		Update3RD(camPos, this);

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
}
