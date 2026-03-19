#pragma once
#ifndef OBJWRITER_HPP
#define OBJWRITER_HPP

#include <vector>
#include <string>
#include "../geometry/AABB.hpp"

using namespace std;

class ObjWriter {
public:
    bool writeVoxels(const string& filepath, const vector<AABB>& voxels);
};

#endif