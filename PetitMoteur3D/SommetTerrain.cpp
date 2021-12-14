#include "stdafx.h"
#include "SommetTerrain.h"


namespace PM3D
{
	// Definir l'organisation de notre sommet
	D3D11_INPUT_ELEMENT_DESC CSommetTerrain::layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT CSommetTerrain::numElements = ARRAYSIZE(CSommetTerrain::layout);

	CSommetTerrain::CSommetTerrain(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& coordTex)
		: m_Position(position)
		, m_Normal(normal)
		, m_CoordTex(coordTex)
	{
	}

} // namespace PM3D