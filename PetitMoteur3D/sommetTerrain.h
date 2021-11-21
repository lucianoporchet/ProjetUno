#pragma once
#include "sommetbloc.h"

namespace PM3D {

	class CSommetTerrain : public CSommetBloc {
	public:
		CSommetTerrain() = default;
		CSommetTerrain(const XMFLOAT3& position, const XMFLOAT3& normal) : CSommetBloc(position, normal) {
			
		}
	};
}