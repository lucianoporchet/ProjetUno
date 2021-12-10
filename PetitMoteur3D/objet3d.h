#pragma once

using namespace DirectX;

namespace PM3D
{
//  Classe : CObjet3D
//
//  BUT : 	Classe de base de tous nos objets 3D
//
class CObjet3D
{
public:
	virtual ~CObjet3D() = default;

	virtual void Anime(float) {};
	virtual void Draw() = 0;
	std::string typeTag;

};

} // namespace PM3D
