// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclure les en-têtes Windows rarement utilisés
// Fichiers d'en-tête Windows :
#include <windows.h>

// Fichiers d'en-tête C RunTime
#include <cassert>
#include <cstdint>
#include <cstring>
#include <tchar.h>

// Fichiers d'en-tête C++ RunTime
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>

// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
#define _XM_NO_INTRINSICS_
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "d3dx11effect.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
