#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "geometry/AABB.hpp"
#include "graphics/Camera.hpp"

class Viewer
{
private:
    sf::RenderWindow window;
    Camera camera;
    std::vector<AABB> voxels;
    bool isDragging;
    sf::Vector2i lastMousePos;

    sf::Font font;
    sf::Text statsText;
    bool shaderEnabled;
    sf::Shader shader;

    void processEvents();
    void render(float deltaTime);

public:
    Viewer(const std::vector<AABB> &voxels);
    void run();
};
