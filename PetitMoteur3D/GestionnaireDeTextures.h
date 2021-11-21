#pragma once

#include "texture.h"

namespace PM3D
{
	class CDispositifD3D11;

	class CGestionnaireDeTextures
	{
	public:
		CTexture* const GetNewTexture(const std::wstring& filename, CDispositifD3D11* pDispositif);
		CTexture* const GetTexture(const std::wstring& filename);

	private:
		// Le tableau de textures
		std::vector<std::unique_ptr<CTexture>> ListeTextures;
	};

} // namespace PM3D
