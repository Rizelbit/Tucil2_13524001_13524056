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
            iss >> i >> j >> k;

            Triangle tri(tempVertices[i-1], tempVertices[j-1], tempVertices[k-1]);
            outTriangles.push_back(tri);
        }
    }
    return true;
}