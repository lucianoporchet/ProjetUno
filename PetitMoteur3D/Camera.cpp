#include "stdafx.h"
#include "Camera.h"

#include "util.h"
#include "DispositifD3D11.h"

#include "resource.h"
#include "MoteurWindows.h"
using namespace DirectX;

namespace PM3D {
	
	void CCamera::UpdateFree(float tempsEcoule) {
		// Matrice de la vision
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie =
			rMoteur.GetGestionnaireDeSaisie();

		// Vérifier l’état de la touche gauche
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_A))
		{
			position -= right * cameraSpeed * tempsEcoule;
			XMStoreFloat3(&pos, position);
			updateView();
		}
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_D))
		{
			position += right * cameraSpeed * tempsEcoule;
			XMStoreFloat3(&pos, position);
			updateView();
		}
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_W))
		{
			position += foward * cameraSpeed * tempsEcoule;
			XMStoreFloat3(&pos, position);
			updateView();
		}
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_S))
		{
			position -= foward * cameraSpeed * tempsEcoule;
			XMStoreFloat3(&pos, position);
			updateView();
		}
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_Z))
		{
			position -= up * cameraSpeed * tempsEcoule;
			XMStoreFloat3(&pos, position);
			updateView();
		}
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_SPACE))
		{
			position += up * cameraSpeed * tempsEcoule;
			XMStoreFloat3(&pos, position);
			updateView();
		}
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_E))
		{
			cameraSpeed += 10.0f;
		}
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_Q))
		{
			cameraSpeed -= 10.0f;
			if (cameraSpeed <= 0.0f)
				cameraSpeed = 10.0f;
		}

		//******** POUR LA SOURIS ************
		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80)) {
			if (rGestionnaireDeSaisie.EtatSouris().lY != 0 || rGestionnaireDeSaisie.EtatSouris().lX != 0)
			{
				yaw -= (float)rGestionnaireDeSaisie.EtatSouris().lX;
				pitch -= (float)rGestionnaireDeSaisie.EtatSouris().lY;
				rot = XMFLOAT3(pitch * 0.00005f, yaw * 0.00005f, 0.0f);
				rotation = XMLoadFloat3(&rot);
				updateView();
			}
		}
	}

	//void CCamera::UpdateLevel(float tempsEcoule) {
	//	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
	//	CDIManipulateur& rGestionnaireDeSaisie =
	//		rMoteur.GetGestionnaireDeSaisie();
	//	// Vérifier l’état de la touche gauche
	//	
	//	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_A))
	//	{
	//		const int x = static_cast<int>(DirectX::XMVectorGetX(position));
	//		const int z = static_cast<int>(XMVectorGetZ(position));
	//		if (x < width && x  >= 0 && z > -static_cast<int>((v_sommets.size() / width)) && z <= 0) {
	//			const float y = v_sommets[std::abs(z) * static_cast<int>(width) + x].getPos().z + levelCamHeight;
	//			position = XMVectorSetY(position, y);
	//			position -= right * cameraSpeed * tempsEcoule;
	//			XMStoreFloat3(&pos, position);
	//			if (pos.x >= width || pos.x < 0 || pos.z <= -static_cast<int>((v_sommets.size() / width)) || pos.z > 0) {
	//				position += right * cameraSpeed * tempsEcoule;
	//				XMStoreFloat3(&pos, position);
	//			}
	//			XMStoreFloat3(&pos, position);
	//			updateView();
	//		}
	//	}
	//	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_D))
	//	{
	//		const int x = static_cast<int>(DirectX::XMVectorGetX(position));
	//		const int z = static_cast<int>(XMVectorGetZ(position));
	//		if (x < width && x >= 0 && z > -static_cast<int>((v_sommets.size() / width)) && z <= 0) {
	//			const float y = v_sommets[std::abs(z) * static_cast<int>(width) + x].getPos().z + levelCamHeight;
	//			position = XMVectorSetY(position, y);
	//			position += right * cameraSpeed * tempsEcoule;
	//			XMStoreFloat3(&pos, position);
	//			if (pos.x >= width || pos.x < 0 || pos.z <= -static_cast<int>((v_sommets.size() / width)) || pos.z > 0) {
	//				position -= right * cameraSpeed * tempsEcoule;
	//				XMStoreFloat3(&pos, position);
	//			}
	//			updateView();
	//		}
	//	}
	//	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_W))
	//	{
	//		
	//		const int x = static_cast<int>(XMVectorGetX(position));
	//		const int z = static_cast<int>(XMVectorGetZ(position));

	//		if (x < width && x >= 0 && z > -static_cast<int>((v_sommets.size() / width)) && z <= 0) {
	//			const float y = v_sommets[std::abs(z) * static_cast<int>(width) + x].getPos().z + levelCamHeight;
	//			position = XMVectorSetY(position, y);
	//			position += foward * cameraSpeed * tempsEcoule;
	//			XMStoreFloat3(&pos, position);
	//			if (pos.x >= width || pos.x < 0 || pos.z <= -static_cast<int>((v_sommets.size() / width)) || pos.z > 0) {
	//				position -= foward * cameraSpeed * tempsEcoule;
	//				XMStoreFloat3(&pos, position);
	//			}
	//			updateView();
	//		}
	//	}
	//	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_S))
	//	{
	//		
	//		const int x = static_cast<int>(XMVectorGetX(position));
	//		const int z = static_cast<int>(XMVectorGetZ(position));

	//		if (x < width && x >= 0 && z > -static_cast<int>((v_sommets.size() / width)) && z <= 0) {
	//			const float y = v_sommets[std::abs(z) * static_cast<int>(width) + x].getPos().z + levelCamHeight;
	//			position = XMVectorSetY(position, y);
	//			position -= foward * cameraSpeed * tempsEcoule;
	//			XMStoreFloat3(&pos, position);
	//			if (pos.x >= width || pos.x < 0 || pos.z <= -static_cast<int>((v_sommets.size() / width)) || pos.z > 0) {
	//				position += foward * cameraSpeed * tempsEcoule;
	//				XMStoreFloat3(&pos, position);
	//			}
	//			updateView();
	//		}
	//	}
	//	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_SUBTRACT))
	//	{
	//		if (levelCamHeight >= 1.00f)
	//		{
	//			levelCamHeight -= 1.00f;
	//		}
	//	}
	//	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_ADD))
	//	{
	//		if (levelCamHeight <= 255.00f)
	//		{
	//			levelCamHeight += 1.00f;
	//		}
	//	}
	//	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_E))
	//	{
	//		if (cameraSpeed <= 10000.f)
	//		{
	//			cameraSpeed += 10.0f;
	//		}
	//		
	//	}
	//	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_Q))
	//	{
	//		cameraSpeed -= 10.0f;
	//		if (cameraSpeed <= 0.0f)
	//			cameraSpeed = 10.0f;
	//	}


	//	//******** POUR LA SOURIS ************
	//	if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80)) {
	//		if (rGestionnaireDeSaisie.EtatSouris().lY != 0 || rGestionnaireDeSaisie.EtatSouris().lX != 0)
	//		{
	//			yaw -= (float)rGestionnaireDeSaisie.EtatSouris().lX;
	//			pitch -= (float)rGestionnaireDeSaisie.EtatSouris().lY;
	//			rot = XMFLOAT3(pitch * 0.005f, yaw * 0.005f, 0.0f);
	//			rotation = XMLoadFloat3(&rot);
	//			updateView();
	//		}
	//	}
	//}

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

		updateView();
	}

	void CCamera::updateCam(XMFLOAT3 camPos, XMFLOAT3 camDir)
	{
		position = XMLoadFloat3(&camPos);
		rot.x = camDir.x;
		rot.y = camDir.y;
		rot.z = camDir.z;
		//direction = XMLoadFloat3(&camDir);
		//updateView();
		updateView();
	}

	void CCamera::updateView() {

		XMMATRIX camRotationM = XMMatrixRotationRollPitchYaw(0.0f, -rot.x, 0.0f);

		direction = XMVector3TransformCoord(DEFAULT_FOWARD, camRotationM);
		direction += position;

		// up never changes for our camera.
		//up = XMVector3TransformCoord(DEFAULT_UP, camRotationM);

		*pMatView = XMMatrixLookAtRH(position,
			direction,
			up);
		*pMatViewProj = (*pMatView) * (*pMatProj);

		XMMATRIX vecRotationM = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		// this does changes though.
		foward = XMVector3TransformCoord(DEFAULT_FOWARD, vecRotationM);
		right = XMVector3TransformCoord(DEFAULT_RIGHT, vecRotationM);
	}



}
