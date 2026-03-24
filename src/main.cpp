#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include "geometry/Octree.hpp"
#include "core/ObjParser.hpp"
#include "core/ObjWriter.hpp"
#include "graphics/Viewer.hpp"

using namespace std;

int main (int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Cara Penggunaan: " << argv[0] << " <path_file_obj> <max_depth>\n";
        cerr << "Contoh: " << argv[0] << " test/pumpkin.obj 4\n";
        return 1;
    }
    string inputPath = argv[1];
    int maxDepth = stoi(argv[2]);

    size_t lastSep = inputPath.find_last_of("/\\");
    string filename = (lastSep == string::npos) ? inputPath : inputPath.substr(lastSep + 1);
    size_t dotPos = filename.find_last_of('.');
    string baseName = (dotPos == string::npos) ? filename : filename.substr(0, dotPos);
    string outputPath = "test/output/" + baseName + "-voxelized.obj";

    cout << "\n=========================================\n";
    cout << "  MEMULAI VOXELIZATION 3D\n";
    cout << "=========================================\n\n";

    // Proses 1: Parsing
    cout << "[1/4] Membaca dan memvalidasi file input (" << inputPath << ")...\n";
    vector<Triangle> triangles;
    Vector3 minBound, maxBound;
    ObjParser parser;
    if (!parser.parse(inputPath, triangles, minBound, maxBound)) {
        return 1;
    }
    cout << "      Selesai! Ditemukan " << triangles.size() << " faces.\n\n";

    Vector3 center = Vector3(
        (minBound.x + maxBound.x) / 2.0f,
        (minBound.y + maxBound.y) / 2.0f,
        (minBound.z + maxBound.z) / 2.0f
    );
    Vector3 halfExtents = Vector3(
        (maxBound.x - minBound.x) / 2.0f,
        (maxBound.y - minBound.y) / 2.0f,
        (maxBound.z - minBound.z) / 2.0f
    );

    float maxHE = std::max({halfExtents.x, halfExtents.y, halfExtents.z});
    Vector3 cubicHalfExtents(maxHE, maxHE, maxHE);
    AABB rootBox(center, cubicHalfExtents);

    // Proses 2: Divide & Conquer
    cout << "[2/4] Membangun Octree dengan kedalaman " << maxDepth << "...\n";
    auto start_time = chrono::high_resolution_clock::now();

    Octree octree(rootBox, maxDepth);
    octree.build(triangles);
    
    // Membersihkan sisa teks progress bar dari Octree.cpp
    cout << "\r   -> Selesai! Total " << octree.getTotalVoxels() << " voxel terbentuk.             \n\n";

    // Proses 3: Ekstraksi Voxel
    cout << "[3/4] Mengambil data Voxel dari daun Octree...\n";
    vector<AABB> finalVoxels = octree.getFinalVoxels();

    // Proses 4: Membuat File Output
    cout << "\n[4/4] Menulis hasil konversi ke file output...\n";
    ObjWriter writer;
    if (!writer.writeVoxels(outputPath, finalVoxels)) {
        cerr << "Gagal menulis ke file output!\n";
        return 1;
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start_time;

    int totalVoxels = octree.getTotalVoxels();
    cout << "\n=== Statistik Voxelization ===\n";
    cout << "Banyaknya voxel yang terbentuk : " << totalVoxels << "\n";
    cout << "Banyaknya vertex yang terbentuk: " << totalVoxels * 8 << "\n";
    cout << "Banyaknya faces yang terbentuk : " << totalVoxels * 12 << "\n";

    cout << "\nStatistik node octree yang terbentuk:\n";
    for (int d = 1; d <= maxDepth; ++d) {
        cout << d << ": " << octree.getNodesFormedAt(d) << "\n";
    }

    cout << "\nStatistik node yang tidak perlu ditelusuri:\n";
    for (int d = 1; d <= maxDepth; ++d) {
        cout << d << ": " << octree.getNodesPrunedAt(d) << "\n";
    }

    cout << "\nKedalaman octree            : " << maxDepth << "\n";
    cout << "Lama waktu program berjalan : " << elapsed.count() << " detik\n";
    cout << "Path file output            : " << outputPath << "\n";

    // Interactive 3D Viewer
    cout << "\nMembuka 3D Viewer...\n";
    Viewer viewer(finalVoxels);
    viewer.run();

    return 0;
}