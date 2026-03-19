#include "Octree.hpp"

Octree::Octree(const AABB& worldBounds, int depthLimit) {
    root = std::make_unique<OctreeNode>(worldBounds);
    maxDepth = depthLimit;
    totalVoxels = 0;

    nodesFormedPerDepth.resize(maxDepth + 1, 0);
    nodesPrunedPerDepth.resize(maxDepth + 1, 0);
}

void Octree::build(const std::vector<Triangle>& objTriangles) {
    subdivideRecursive(root.get(), objTriangles, 0);
}

void Octree::subdivideRecursive(OctreeNode* node, const std::vector<Triangle>& triangles, int currentDepth) {
    
    std::vector<Triangle> intersectingTriangles;
    for (const auto& tri : triangles) {
        if (node->boundingBox.intersects(tri)) {
            intersectingTriangles.push_back(tri);
        }
    }

    // Basis 1: Pruning
    if (intersectingTriangles.empty()) {
        if (currentDepth > 0 && currentDepth <= maxDepth) {
            nodesPrunedPerDepth[currentDepth]++;
        }
        return; 
    }

    // Basis 2: Voxel
    if (currentDepth == maxDepth) {
        totalVoxels++;
        return; 
    }

    node->isLeaf = false; 
    
    Vector3 newHE = node->boundingBox.halfExtents * 0.5f;
    Vector3 c = node->boundingBox.center;
    Vector3 offsets[8] = {
        Vector3(-1, -1, -1), Vector3( 1, -1, -1),
        Vector3(-1,  1, -1), Vector3( 1,  1, -1),
        Vector3(-1, -1,  1), Vector3( 1, -1,  1),
        Vector3(-1,  1,  1), Vector3( 1,  1,  1)
    };

    int nextDepth = currentDepth + 1;
    
    if (nextDepth <= maxDepth) {
        nodesFormedPerDepth[nextDepth] += 8; 
    }

    for (int i = 0; i < 8; ++i) {
        // Child center
        Vector3 childCenter(
            c.x + offsets[i].x * newHE.x,
            c.y + offsets[i].y * newHE.y,
            c.z + offsets[i].z * newHE.z
        );
        
        AABB childBox(childCenter, newHE);
        node->children[i] = std::make_unique<OctreeNode>(childBox);
        
        // Rekursi
        subdivideRecursive(node->children[i].get(), intersectingTriangles, nextDepth);
    }
}