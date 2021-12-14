#include "stdafx.h"
#include "TunnelComponent.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"


TunnelComponent::TunnelComponent(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, PxVec3 scale, PxVec3 pos, PxQuat quat)
	: CObjetMesh(nomfichier, _pDispositif, 1.0f)
{
	body = PhysXManager::get().createStatic(PxTransform(pos), PxBoxGeometry(scale.x, scale.y, scale.z), scene);

	const XMFLOAT3 posF3(pos.x, pos.y, pos.z);
	const XMVECTOR posVec = XMLoadFloat3(&posF3);
	body->setGlobalPose(PxTransform(pos, quat));
	const XMFLOAT4 quatF3(quat.x, quat.y, quat.z, quat.w);
	const XMVECTOR quatVec = XMLoadFloat4(&quatF3);
	setMatWorld(XMMatrixScaling(scale.x, scale.y, scale.z)* XMMatrixRotationQuaternion(quatVec)* XMMatrixTranslationFromVector(posVec));


}


void TunnelComponent::Anime(float tempEcoule)
{
}


