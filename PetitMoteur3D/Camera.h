#pragma once
#include "objet3d.h"
#include "d3dx11effect.h"
#include "sommetbloc.h"
#include <vector>


namespace PM3D {

	class CCamera
	{
	private:
		XMVECTOR position;
		XMVECTOR direction;
		int cursorPosx;
		int cursorPosy;
		
		XMVECTOR rotation;
		
		XMVECTOR right;

		XMFLOAT3 rot{ 0.0f, 0.0f, 0.0f }, pos;

		std::vector<CSommetBloc> v_sommets;
		float width, pitch{ 0.0f }, yaw{ 0.0f };

		const XMVECTOR DEFAULT_FOWARD = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		const XMVECTOR DEFAULT_UP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const XMVECTOR DEFAULT_RIGHT = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		
		float cameraSpeed = 100.0f;
		float levelCamHeight = 15.0f;
		
	public:
		CCamera() = default;
		CCamera(const XMVECTOR& position_in,
			const XMVECTOR& direction_in,
			const XMVECTOR& up_in,
			XMMATRIX* pMatView_in,
			XMMATRIX* pMatProj_in,
			XMMATRIX* pMatViewProj_in);
		void Init(const XMVECTOR& position_in,
			const XMVECTOR& direction_in,
			const XMVECTOR& up_in,
			XMMATRIX* pMatView_in,
			XMMATRIX* pMatProj_in,
			XMMATRIX* pMatViewProj_in);
		void updateCam(XMFLOAT3 camPos);
		void UpdateFree(float);
		void UpdateLevel(float);
		inline void SetPosition(const XMVECTOR& position_in) { position = position_in; };
		inline void SetDirection(const XMVECTOR& direction_in) { direction = direction_in; }
		inline void SetUp(const XMVECTOR& up_in) { up = up_in; }
		inline const XMVECTOR& getPosition() { return position; };
		inline void setSommets(const std::vector<CSommetBloc>& sommets, const float& w) { 
			v_sommets = sommets;
			width = w;
		}
		void updateView();

	public:
		XMMATRIX* pMatView;
		XMMATRIX* pMatProj;
		XMMATRIX* pMatViewProj;
		//XMMATRIX matWorld;
		float angleDirectionCamera;

		XMVECTOR foward;
		XMVECTOR up;
	};
}

