#pragma once
#include <fstream>
#include <vector>
#include <DirectXMath.h>
#include <fstream>
#include <iostream>
#include <tuple>

using namespace std;
using namespace DirectX;


// CLASS LectureFichier
// Permet de lire les .obj, uniquement au format suivant :
// v X Y Z
// vn X Y Z
// f # # #
// avec les indices commencant a 0
// Tres clairement, une generalisation est possible pour au moins ignorer les commentaires
// MAIS on a pas le temps.
class LectureFichier
{
private:
	fstream data;
	vector<long> faces;
	vector<XMFLOAT3> vertexes;
	vector<XMFLOAT3> normals;

public:
	LectureFichier(string filePath);
	LectureFichier(const LectureFichier& other);

	bool open(string filepath);
	bool close();
	bool readFile();

	void serializeObject(ofstream& outputStream);
	void deserializeObject(ifstream& inputStream);

	inline const vector<long>& getFaces() noexcept { return faces; }
	inline const vector<XMFLOAT3>& getVertexes() noexcept { return vertexes; }
	inline const vector<XMFLOAT3>& getNormals() noexcept { return normals; }
};

