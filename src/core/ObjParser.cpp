#include "ObjParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

bool ObjParser::parse(const string& filepath, vector<Triangle>& outTriangles, Vector3& outMinBound, Vector3& outMaxBound) {
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "Gagal membuka file input di: " << filepath << endl;
        return false;
    }

    vector<Vector3> tempVertices;
    string line;

    outMinBound = Vector3(1e9, 1e9, 1e9);
    outMaxBound = Vector3(-1e9, -1e9, -1e9);

    while (getline(file, line)) {
        istringstream iss(line);
        string type;
        iss >> type;

        if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            Vector3 v(x, y, z);
            tempVertices.push_back(v);

            outMinBound.x = min(outMinBound.x, x);
            outMinBound.y = min(outMinBound.y, y);
            outMinBound.z = min(outMinBound.z, z);
            outMaxBound.x = max(outMaxBound.x, x);
            outMaxBound.y = max(outMaxBound.y, y);
            outMaxBound.z = max(outMaxBound.z, z);
        } else if (type == "f") {
            int i, j, k;
            if (!(iss >> i >> j >> k)) {
                cerr << "Error: Format faces (f) tidak valid pada baris.\n";
                return false;
            }

            if (i < 1 || j < 1 || k < 1 || 
                (size_t)i > tempVertices.size() || (size_t)j > tempVertices.size() || (size_t)k > tempVertices.size()) {
                cerr << "Error: Index vertex pada (f) di luar batas.\n";
                return false;
            }

            Triangle tri(tempVertices[i-1], tempVertices[j-1], tempVertices[k-1]);
            outTriangles.push_back(tri);
        } else if (type != "v" && type != "f" && type != "#" && type != "" && type != "g" && type != "vn") {
             cerr << "Error: Input Tidak Valid. Terdapat format yang tidak sesuai: " << type << "\n";
             return false;
        }
    }
    return true;
}