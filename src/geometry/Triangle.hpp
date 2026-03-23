#pragma once
#include "../math/Vector3.hpp"

struct Triangle {
    Vector3 v0, v1, v2;

    Triangle(const Vector3& a, const Vector3& b, const Vector3& c) : v0(a), v1(b), v2(c) {}
};