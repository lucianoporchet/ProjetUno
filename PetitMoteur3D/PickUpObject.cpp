#include "stdafx.h"
#include "PickUpObject.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"


PickUpObject::PickUpObject(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif,PickUpObjectType objType, float scale, PxVec3 pos, int scene)
	: CObjetMesh(nomfichier, _pDispositif, scale), scale{ scale }, objectType{ objType }
{
	body = PhysXManager::get().createDynamic(PxTransform(pos), PxSphereGeometry(scale), PxVec3(0, 0, 0), scene);

	const XMFLOAT3 posF3(pos.x, pos.y, pos.z);
	const XMVECTOR posVec = XMLoadFloat3(&posF3);

	const PxQuat quat = body->getGlobalPose().q;
	const XMFLOAT4 quatF3(quat.x, quat.y, quat.z, quat.w);
	const XMVECTOR quatVec = XMLoadFloat4(&quatF3);
	setMatWorld(XMMatrixScaling(scale, scale, scale)* XMMatrixRotationQuaternion(quatVec)* XMMatrixTranslationFromVector(posVec));

	PhysXManager::get().setupFiltering(body, FilterGroup::ePickupObject, FilterGroup::ePlayer);


}


void PickUpObject::Anime(float tempEcoule)
{
}

PickUpObjectType PickUpObject::getType()
{
	return objectType;
}
