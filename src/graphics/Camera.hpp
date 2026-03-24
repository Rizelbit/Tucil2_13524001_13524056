#pragma once
#include "math/Vector3.hpp"
#include "math/Matrix4.hpp"

class Camera
{
private:
    Vector3 target;
    float radius;
    float theta; // Azimuth angle
    float phi;   // Polar angle
    float fov;
    float aspectRatio;

    Vector3 getPosition() const;

public:
    Camera(Vector3 target = Vector3(0, 0, 0), float radius = 5.0f);
    Vector3 getPositionWorld() const { return getPosition(); }

    void orbit(float deltaTheta, float deltaPhi);
    void zoom(float amount);
    void setAspectRatio(float aspect);

    Matrix4 getViewMatrix() const;
    Matrix4 getProjectionMatrix() const;
};
