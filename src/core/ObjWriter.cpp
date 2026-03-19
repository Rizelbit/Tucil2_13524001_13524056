#include "ObjWriter.hpp"
#include <fstream>
#include <iostream>

bool ObjWriter::writeVoxels(const string& filepath, const vector<AABB>& voxels) {
    ofstream file(filepath);
    if (!file.is_open()) {
        cerr << "Gagal membuat file output di: " << filepath << endl;
        return false;
    }

    int vertexOffset = 1;

    for (const auto& voxel : voxels) {
        Vector3 c = voxel.center;
        Vector3 h = voxel.halfExtents;

        // Bawah (y - h.y)
        file << "v " << c.x - h.x << " " << c.y - h.y << " " << c.z - h.z << "\n";
        file << "v " << c.x + h.x << " " << c.y - h.y << " " << c.z - h.z << "\n";
        file << "v " << c.x + h.x << " " << c.y - h.y << " " << c.z + h.z << "\n";
        file << "v " << c.x - h.x << " " << c.y - h.y << " " << c.z + h.z << "\n";
        // Atas (y + h.y)
        file << "v " << c.x - h.x << " " << c.y + h.y << " " << c.z - h.z << "\n";
        file << "v " << c.x + h.x << " " << c.y + h.y << " " << c.z - h.z << "\n";
        file << "v " << c.x + h.x << " " << c.y + h.y << " " << c.z + h.z << "\n";
        file << "v " << c.x - h.x << " " << c.y + h.y << " " << c.z + h.z << "\n";

        int v[8];
        for (int i = 0; i < 8;i++) v[i] = vertexOffset + i;

        // Sisi Bawah dan Atas
        file << "f " << v[0] << " " << v[2] << " " << v[1] << "\n";
        file << "f " << v[0] << " " << v[3] << " " << v[2] << "\n";
        file << "f " << v[4] << " " << v[5] << " " << v[6] << "\n";
        file << "f " << v[4] << " " << v[6] << " " << v[7] << "\n";
        // Sisi Depan dan Belakang
        file << "f " << v[3] << " " << v[6] << " " << v[2] << "\n";
        file << "f " << v[3] << " " << v[7] << " " << v[6] << "\n";
        file << "f " << v[0] << " " << v[1] << " " << v[5] << "\n";
        file << "f " << v[0] << " " << v[5] << " " << v[4] << "\n";
        // Sisi Kiri dan Kanan
        file << "f " << v[0] << " " << v[4] << " " << v[7] << "\n";
        file << "f " << v[0] << " " << v[7] << " " << v[3] << "\n";
        file << "f " << v[1] << " " << v[2] << " " << v[6] << "\n";
        file << "f " << v[1] << " " << v[6] << " " << v[5] << "\n";

        vertexOffset += 8;
    }

    file.close();
    return true;
}