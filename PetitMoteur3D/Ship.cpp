#include "stdafx.h"
#include "Ship.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"


Ship::Ship(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale)
	: CObjetMesh(nomfichier, _pDispositif, scale), scale{ scale }
{
	mDir = PxVec3(0, 0, -1);
}

void Ship::Anime(float tempEcoule)
{
	const PxVec3 pos = body->getGlobalPose().p;
	const XMFLOAT3 posF3(pos.x, pos.y, pos.z);
	const XMVECTOR posVec = XMLoadFloat3(&posF3);

	const PxQuat quat = body->getGlobalPose().q;
	const XMFLOAT4 quatF3(quat.x, quat.y, quat.z, quat.w);
	const XMVECTOR quatVec = XMLoadFloat4(&quatF3);

	setMatWorld(XMMatrixScaling(scale, scale, scale) * XMMatrixRotationQuaternion(quatVec) * XMMatrixTranslationFromVector(posVec));
}


