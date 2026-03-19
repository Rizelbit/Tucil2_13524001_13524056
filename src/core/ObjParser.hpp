#pragma once
#ifndef OBJPARSER_HPP
#define OBJPARSER_HPP

#include <string>
#include <vector>
#include "../geometry/Triangle.hpp"
#include "../math/Vector3.hpp"

using namespace std;

class ObjParser {
public:
    bool parse(const string& filepath, vector<Triangle>& outTriangles, Vector3& outMinBound, Vector3& outMaxBound);
};

#endif