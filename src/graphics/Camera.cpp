#include "graphics/Camera.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera(Vector3 target, float radius) 
    : target(target), radius(radius), theta(0), phi(M_PI / 4.0f), fov(45.0f * M_PI / 180.0f), aspectRatio(1.0f) {}

Vector3 Camera::getPosition() const {
    return Vector3(
        target.x + radius * sinf(phi) * sinf(theta),
        target.y + radius * cosf(phi),
        target.z + radius * sinf(phi) * cosf(theta)
    );
}

void Camera::orbit(float deltaTheta, float deltaPhi) {
    theta += deltaTheta;
    phi += deltaPhi;

    // Batasi phi untuk menghindari flipping
    const float margin = 0.1f;
    phi = std::max(margin, std::min((float)M_PI - margin, phi));
}

void Camera::zoom(float amount) {
    // Zoom relatif terhadap radius saat ini agar kecepatan konsisten di semua skala
    radius += amount * radius * 0.15f;
    if (radius < 0.1f) radius = 0.1f;
}

void Camera::setAspectRatio(float aspect) {
    aspectRatio = aspect;
}

Matrix4 Camera::getViewMatrix() const {
    return Matrix4::createLookAt(getPosition(), target, Vector3(0, 1, 0));
}

Matrix4 Camera::getProjectionMatrix() const {
    return Matrix4::createPerspective(fov, aspectRatio, 0.1f, 1000.0f);
}
