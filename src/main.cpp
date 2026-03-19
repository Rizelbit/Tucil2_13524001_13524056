#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include "geometry/Octree.hpp"
#include "core/ObjParser.hpp"
#include "core/ObjWriter.hpp"

using namespace std;

int main (int argc, char* argv[]) {
    string filename = argv[1];
    int maxDepth = stoi(argv[2]);

    string inputPath = "test/" + filename;
    size_t dotPos = filename.find_last_of('.');
    string baseName = (dotPos == string::npos) ? filename : filename.substr(0, dotPos);
    string outputPath = "test/" + baseName + "-voxelized.obj";

    auto start_time = chrono::high_resolution_clock::now();

    vector<Triangle> triangles;
    Vector3 minBound, maxBound;
    ObjParser parser;
    if (!parser.parse(inputPath, triangles, minBound, maxBound)) {
        return 1;
    }

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
    AABB rootBox(center, halfExtents);

    Octree octree(rootBox, maxDepth);
    octree.build(triangles);

    vector<AABB> finalVoxels = octree.getFinalVoxels();

    ObjWriter writer;
    writer.writeVoxels(outputPath, finalVoxels);

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

    return 0;
}