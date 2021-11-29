#include "stdafx.h"
#include "SceneManager.h"

std::vector<std::unique_ptr<PM3D::CObjet3D>>& SceneManager::getListScene()
{
	return ListeScene;
}
