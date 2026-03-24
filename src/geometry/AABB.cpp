// src/geometry/AABB.cpp
#include "AABB.hpp"
#include <algorithm>
#include <cmath>

// Proyeksi AABB
float projectAABB(const Vector3 &halfExtents, const Vector3 &axis)
{
    return halfExtents.x * std::abs(axis.x) +
           halfExtents.y * std::abs(axis.y) +
           halfExtents.z * std::abs(axis.z);
}

// SAT Intersection
bool AABB::intersects(const Triangle &tri) const
{
    Vector3 v0 = tri.v0 - center;
    Vector3 v1 = tri.v1 - center;
    Vector3 v2 = tri.v2 - center;

    Vector3 e0 = v1 - v0;
    Vector3 e1 = v2 - v1;
    Vector3 e2 = v0 - v2;

    float minX = std::min({v0.x, v1.x, v2.x});
    float maxX = std::max({v0.x, v1.x, v2.x});
    if (minX > halfExtents.x || maxX < -halfExtents.x)
        return false;

    float minY = std::min({v0.y, v1.y, v2.y});
    float maxY = std::max({v0.y, v1.y, v2.y});
    if (minY > halfExtents.y || maxY < -halfExtents.y)
        return false;

    float minZ = std::min({v0.z, v1.z, v2.z});
    float maxZ = std::max({v0.z, v1.z, v2.z});
    if (minZ > halfExtents.z || maxZ < -halfExtents.z)
        return false;

    // Uji Normal Segitiga
    Vector3 triangleNormal = e0.cross(e1);

    // Jarak Bidang
    float planeDistance = triangleNormal.dot(v0);

    // Proyeksi AABB
    float r = projectAABB(halfExtents, triangleNormal);

    // Cek Pemisah
    if (std::abs(planeDistance) > r)
        return false;

    float p0, p1, p2, rad, minP, maxP;
    Vector3 axis;

    // Array 9 Sumbu
    Vector3 crossAxes[9] = {
        Vector3(0, -e0.z, e0.y), Vector3(0, -e1.z, e1.y), Vector3(0, -e2.z, e2.y), // Sumbu X
        Vector3(e0.z, 0, -e0.x), Vector3(e1.z, 0, -e1.x), Vector3(e2.z, 0, -e2.x), // Sumbu Y
        Vector3(-e0.y, e0.x, 0), Vector3(-e1.y, e1.x, 0), Vector3(-e2.y, e2.x, 0)  // Sumbu Z
    };

    for (int i = 0; i < 9; ++i)
    {
        axis = crossAxes[i];

        // Proyeksi Segitiga
        p0 = v0.dot(axis);
        p1 = v1.dot(axis);
        p2 = v2.dot(axis);

        minP = std::min({p0, p1, p2});
        maxP = std::max({p0, p1, p2});

        // Proyeksi AABB
        rad = projectAABB(halfExtents, axis);

        if (minP > rad || maxP < -rad)
            return false;
    }

    return true;
}

bool AABB::intersectsRay(const Vector3 &rayOrigin, const Vector3 &rayDir, float &tNear) const
{
    // Slab method
    Vector3 minPt = center - halfExtents;
    Vector3 maxPt = center + halfExtents;

    float t1 = (minPt.x - rayOrigin.x) / rayDir.x;
    float t2 = (maxPt.x - rayOrigin.x) / rayDir.x;
    float tmin = std::min(t1, t2);
    float tmax = std::max(t1, t2);

    t1 = (minPt.y - rayOrigin.y) / rayDir.y;
    t2 = (maxPt.y - rayOrigin.y) / rayDir.y;
    tmin = std::max(tmin, std::min(t1, t2));
    tmax = std::min(tmax, std::max(t1, t2));

    t1 = (minPt.z - rayOrigin.z) / rayDir.z;
    t2 = (maxPt.z - rayOrigin.z) / rayDir.z;
    tmin = std::max(tmin, std::min(t1, t2));
    tmax = std::min(tmax, std::max(t1, t2));

    if (tmax >= std::max(0.0f, tmin))
    {
        tNear = tmin;
        return true;
    }
    return false;
}