
#include "StdAfx.h"
#include "dispositifD3D11.h"
#include "GestionnaireDeTextures.h"
#include <mutex>

namespace PM3D
{

static std::mutex textureMutex;
CTexture* const CGestionnaireDeTextures::GetNewTexture(const std::wstring& filename, CDispositifD3D11* pDispositif)
{
	std::lock_guard<std::mutex> lock(textureMutex);
	// On vérifie si la texture est déjà dans notre liste
	CTexture* pTexture = GetTexture(filename);

	// Si non, on la crée
	if (!pTexture)
	{
		auto texture = std::make_unique<CTexture>(filename, pDispositif);
		pTexture = texture.get();
		// Puis, il est ajouté à la scène
		ListeTextures.push_back(std::move(texture));
	}

	assert(pTexture);
	return pTexture;
}

CTexture* const CGestionnaireDeTextures::GetTexture(const std::wstring& filename)
{
	CTexture* pTexture = nullptr;

	for (auto& texture : ListeTextures)
	{
		if (texture->GetFilename() == filename)
		{
			pTexture = texture.get();
			break;
		}
	}

	return pTexture;
}

} // namespace PM3D
