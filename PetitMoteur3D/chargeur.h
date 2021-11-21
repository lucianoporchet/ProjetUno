#pragma once

using namespace DirectX;

namespace PM3D
{

struct CParametresChargement
{
	CParametresChargement()
		: bInverserCulling(false)
		, bMainGauche(false)
	{
	}

	std::string NomFichier;
	std::string NomChemin;
	bool bInverserCulling;
	bool bMainGauche;
};

class IChargeur
{
public:
	virtual ~IChargeur() = default;

	virtual void Chargement(const CParametresChargement& param) = 0;

	virtual size_t GetNombreSommets() const = 0;
	virtual size_t GetNombreIndex() const = 0;
	virtual const void* GetIndexData()  const = 0;
	virtual int GetNombreSubset() const = 0;
	virtual size_t  GetNombreMaterial() const = 0;
	virtual void GetMaterial(int _i,
		std::string& _NomFichierTexture,
		std::string& _NomMateriau,
		XMFLOAT4& _Ambient,
		XMFLOAT4& _Diffuse,
		XMFLOAT4& _Specular,
		float& _Puissance) const = 0;

	virtual const std::string& GetMaterialName(int i) const = 0;

	virtual void CopieSubsetIndex(std::vector<int>& dest) const = 0;

	virtual const XMFLOAT3& GetPosition(int NoSommet) const = 0;
	virtual const XMFLOAT2& GetCoordTex(int NoSommet) const = 0;
	virtual const XMFLOAT3& GetNormale(int NoSommet) const = 0;
};

} // namespace PM3D
