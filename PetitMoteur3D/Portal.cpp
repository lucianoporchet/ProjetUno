#include "stdafx.h"
#include "Portal.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"


Portal::Portal(const std::string& nomfichier, PM3D::CDispositifD3D11* _pDispositif, float scale, PxVec3 pos, int scene)
	: CObjetMesh(nomfichier, _pDispositif, scale), scale{ scale }
{
	body = PhysXManager::get().createDynamic(PxTransform(pos), PxSphereGeometry(scale), PxVec3(0, 0, 0), scene);

	const XMFLOAT3 posF3(pos.x, pos.y, pos.z);
	const XMVECTOR posVec = XMLoadFloat3(&posF3);

	const PxQuat quat = body->getGlobalPose().q;
	const XMFLOAT4 quatF3(quat.x, quat.y, quat.z, quat.w);
	const XMVECTOR quatVec = XMLoadFloat4(&quatF3);
	setMatWorld(XMMatrixScaling(scale, scale, scale)* XMMatrixRotationQuaternion(quatVec)* XMMatrixTranslationFromVector(posVec));

	switch (scene) {
	case 0:
		PhysXManager::get().setupFiltering(body,
			FilterGroup::ePortal1to2,
			FilterGroup::ePlayer);
		break;
	case 1:
		PhysXManager::get().setupFiltering(body,
			FilterGroup::ePortal2to1,
			FilterGroup::ePlayer);
		break;
	case 2:
		PhysXManager::get().setupFiltering(body,
			FilterGroup::ePortal2to3,
			FilterGroup::ePlayer);
		break;
	case 3:
		PhysXManager::get().setupFiltering(body,
			FilterGroup::ePortal3to2,
			FilterGroup::ePlayer);
	case 4:
		PhysXManager::get().setupFiltering(body,
			FilterGroup::ePortal3to4,
			FilterGroup::ePlayer);
		break;
	case 5:
		PhysXManager::get().setupFiltering(body,
			FilterGroup::ePortal4to3,
			FilterGroup::ePlayer);
		break;
	case 6:
		PhysXManager::get().setupFiltering(body,
			FilterGroup::ePortal4to1,
			FilterGroup::ePlayer);
		break;
	case 7:
		PhysXManager::get().setupFiltering(body,
			FilterGroup::ePortalEnd,
			FilterGroup::ePlayer);
		break;
	default:
		break;
	}
	
}


void Portal::Anime(float tempEcoule)
{
}


