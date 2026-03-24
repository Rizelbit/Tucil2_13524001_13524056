#pragma once
#include "math/Vector3.hpp"
#include <vector>

struct Matrix4 {
    float m[4][4];

    Matrix4();

    static Matrix4 identity();
    static Matrix4 createTranslation(const Vector3& translation);
    static Matrix4 createRotationX(float angle);
    static Matrix4 createRotationY(float angle);
    static Matrix4 createRotationZ(float angle);
    static Matrix4 createPerspective(float fov, float aspect, float near, float far);
    static Matrix4 createLookAt(const Vector3& eye, const Vector3& target, const Vector3& up);

    Matrix4 operator*(const Matrix4& other) const;
    Vector3 multiplyVector(const Vector3& v) const;
};
