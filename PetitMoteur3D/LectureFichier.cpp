#include "stdafx.h"
#include "LectureFichier.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <array>

using namespace std;
using namespace DirectX;

template<typename POD>
std::ostream& serialize(std::ostream& os, std::vector<POD> const& v)
{
    // this only works on built in data types (PODs)
    static_assert(std::is_trivial<POD>::value && std::is_standard_layout<POD>::value,
        "Can only serialize POD types with this function");

    const auto size = v.size();
    os.write(reinterpret_cast<char const*>(&size), sizeof(size));
    os.write(reinterpret_cast<char const*>(v.data()), v.size() * sizeof(POD));
    return os;
}

template<typename POD>
std::istream& deserialize(std::istream& is, std::vector<POD>& v)
{
    static_assert(std::is_trivial<POD>::value && std::is_standard_layout<POD>::value,
        "Can only deserialize POD types with this function");

    decltype(v.size()) size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    v.resize(size);
    is.read(reinterpret_cast<char*>(v.data()), v.size() * sizeof(POD));
    return is;
}

struct IncorrectObjectFileFormat {};

LectureFichier::LectureFichier(string filePath)
{
    string dataFilePath = filePath + ".data";
    ifstream dataFile = ifstream(dataFilePath, ios::binary);
    bool dataExists = false;
    if (dataFile) {//check if .data file exists
        deserializeObject(dataFile); // deserialize data
        dataExists = true;
        dataFile.close();
    }
    else
    {
        open(filePath + ".obj");
        readFile();
        close();
    }
    if (!dataExists)
    {
        ofstream dataFileWrite = ofstream(dataFilePath, ios::binary);
        serializeObject(dataFileWrite);// serialize data, for another run later
        dataFileWrite.close();
    }
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

void LectureFichier::serializeObject(ofstream& outputStream)
{
    serialize(outputStream, faces);
    serialize(outputStream, vertexes);
    serialize(outputStream, normals);
}

void LectureFichier::deserializeObject(ifstream& inputStream)
{
    deserialize(inputStream, faces);
    deserialize(inputStream, vertexes);
    deserialize(inputStream, normals);
}
