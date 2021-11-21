#include "stdafx.h"
#include "LectureFichier.h"
#include <fstream>
#include <string>
#include <sstream>
#include <array>

using namespace std;
using namespace DirectX;

struct IncorrectObjectFileFormat {};

LectureFichier::LectureFichier(string filePath)
{
    open(filePath);
    readFile();
    close();
}

LectureFichier::LectureFichier(const LectureFichier& other)
{
    faces = other.faces;
    vertexes = other.vertexes;
    normals = other.normals;
}

bool LectureFichier::open(string filepath)
{
    data.open(filepath);

    return data.is_open();
}

bool LectureFichier::close()
{
    if (data.is_open()) {
        data.close();
    }
    return !data.is_open();
}

bool LectureFichier::readFile()
{
    if (data.is_open()) {
        char line[256];

        while (data) {

            data.getline(line, 256, '\n');

            istringstream input{ line };
            vector<string> contents;
            for (string a; input.getline(&a[0], 256, ' '); ) {
                contents.push_back(a);
            }

            if (line[0] == 'v') {

                switch (line[1]) {
                case ' ':
                    vertexes.push_back(XMFLOAT3{ stof(contents[1]), stof(contents[2]), stof(contents[3]) });
                    break;

                case 'n':
                    normals.push_back(XMFLOAT3{ stof(contents[1]), stof(contents[2]), stof(contents[3]) });
                    break;
                }
            }
            else if (line[0] == 'f') {
                faces.push_back(stol(contents[1])-1);
                faces.push_back(stol(contents[2])-1);
                faces.push_back(stol(contents[3])-1);
            }
            else if (line[0] == '\0') {
                break;
            }
            else {
                // yup, on throw a la moindre modif dans le format. Oupsie. Comme dit dans le .h, on a pas eu le temps de generaliser
                throw IncorrectObjectFileFormat{};
            }
        }

        data.close();
    }



    return false;
}
