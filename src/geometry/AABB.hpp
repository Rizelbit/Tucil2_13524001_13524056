#pragma once
#include "../math/Vector3.hpp"
#include "Triangle.hpp"

struct AABB
{
    Vector3 center;
    Vector3 halfExtents;

    AABB() {}
    AABB(const Vector3 &c, const Vector3 &he) : center(c), halfExtents(he) {}

    // Cek Irisan
    bool intersects(const Triangle &tri) const;
    bool intersectsRay(const Vector3 &rayOrigin, const Vector3 &rayDir, float &tNear) const;
};