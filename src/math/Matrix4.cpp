#include "Matrix4.hpp"
#include <cmath>
#include <cstring>

Matrix4::Matrix4() {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] = 0;
}

Matrix4 Matrix4::identity() {
    Matrix4 res;
    for (int i = 0; i < 4; ++i) res.m[i][i] = 1.0f;
    return res;
}

Matrix4 Matrix4::createTranslation(const Vector3& translation) {
    Matrix4 res = identity();
    res.m[0][3] = translation.x;
    res.m[1][3] = translation.y;
    res.m[2][3] = translation.z;
    return res;
}

Matrix4 Matrix4::createRotationX(float angle) {
    Matrix4 res = identity();
    float c = cosf(angle);
    float s = sinf(angle);
    res.m[1][1] = c; res.m[1][2] = -s;
    res.m[2][1] = s; res.m[2][2] = c;
    return res;
}

Matrix4 Matrix4::createRotationY(float angle) {
    Matrix4 res = identity();
    float c = cosf(angle);
    float s = sinf(angle);
    res.m[0][0] = c;  res.m[0][2] = s;
    res.m[2][0] = -s; res.m[2][2] = c;
    return res;
}

Matrix4 Matrix4::createRotationZ(float angle) {
    Matrix4 res = identity();
    float c = cosf(angle);
    float s = sinf(angle);
    res.m[0][0] = c; res.m[0][1] = -s;
    res.m[1][0] = s; res.m[1][1] = c;
    return res;
}

Matrix4 Matrix4::createPerspective(float fov, float aspect, float near, float far) {
    Matrix4 res;
    float tanHalfFov = tanf(fov / 2.0f);
    res.m[0][0] = 1.0f / (aspect * tanHalfFov);
    res.m[1][1] = 1.0f / tanHalfFov;
    res.m[2][2] = -(far + near) / (far - near);
    res.m[2][3] = -(2.0f * far * near) / (far - near);
    res.m[3][2] = -1.0f;
    return res;
}

Matrix4 Matrix4::createLookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
    Vector3 f = (target - eye);
    float lenF = sqrtf(f.dot(f));
    f = f * (1.0f / lenF);

    Vector3 u_norm = up;
    float lenUp = sqrtf(up.dot(up));
    u_norm = u_norm * (1.0f / lenUp);

    Vector3 s = f.cross(u_norm);
    float lenS = sqrtf(s.dot(s));
    s = s * (1.0f / lenS);

    Vector3 u = s.cross(f);

    Matrix4 res = identity();
    res.m[0][0] = s.x; res.m[0][1] = s.y; res.m[0][2] = s.z;
    res.m[1][0] = u.x; res.m[1][1] = u.y; res.m[1][2] = u.z;
    res.m[2][0] = -f.x; res.m[2][1] = -f.y; res.m[2][2] = -f.z;

    res.m[0][3] = -s.dot(eye);
    res.m[1][3] = -u.dot(eye);
    res.m[2][3] = f.dot(eye);

    return res;
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 res;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                res.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return res;
}

Vector3 Matrix4::multiplyVector(const Vector3& v) const {
    float x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3];
    float y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3];
    float z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3];
    float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3];

    if (w != 0 && w != 1.0f) {
        return Vector3(x / w, y / w, z / w);
    }
    return Vector3(x, y, z);
}
