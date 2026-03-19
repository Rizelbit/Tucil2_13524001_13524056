#pragma once
#include <vector>
#include <memory>
#include "AABB.hpp"
#include "Triangle.hpp"

struct OctreeNode {
    AABB boundingBox;
    bool isLeaf;
    std::unique_ptr<OctreeNode> children[8];

    OctreeNode(const AABB& box) : boundingBox(box), isLeaf(true) {}
};

class Octree {
private:
    std::unique_ptr<OctreeNode> root;
    int maxDepth;
    int totalVoxels;
    std::vector<int> nodesFormedPerDepth;
    std::vector<int> nodesPrunedPerDepth;
    void subdivideRecursive(OctreeNode* node, const std::vector<Triangle>& triangles, int currentDepth);
    void extractVoxelsRecursive(OctreeNode* node, std::vector<AABB>& outVoxels, int currentDepth);

public:
    Octree(const AABB& worldBounds, int depthLimit);

    void build(const std::vector<Triangle>& objTriangles);

    std::vector<AABB> getFinalVoxels();

    // Getter
    int getTotalVoxels() const;
    int getNodesFormedAt(int depth) const;
    int getNodesPrunedAt(int depth) const;
};