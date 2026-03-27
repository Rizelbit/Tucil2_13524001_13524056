#include "graphics/Viewer.hpp"
#include <iostream>
#include <algorithm>
#include <cfloat>

Viewer::Viewer(const std::vector<AABB> &voxels) : voxels(voxels), isDragging(false), shaderEnabled(false)
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(800, 600), "3D Voxel Viewer", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    // Initial camera setup: target at scene center
    Vector3 sceneCenter(0, 0, 0);
    float sceneRadius = 5.0f;

    if (!voxels.empty())
    {
        Vector3 minCorner = voxels[0].center;
        Vector3 maxCorner = voxels[0].center;

        for (const AABB &v : voxels)
        {
            minCorner.x = std::min(minCorner.x, v.center.x);
            minCorner.y = std::min(minCorner.y, v.center.y);
            minCorner.z = std::min(minCorner.z, v.center.z);
            maxCorner.x = std::max(maxCorner.x, v.center.x);
            maxCorner.y = std::max(maxCorner.y, v.center.y);
            maxCorner.z = std::max(maxCorner.z, v.center.z);
        }

        sceneCenter = Vector3((minCorner.x + maxCorner.x) * 0.5f,
                              (minCorner.y + maxCorner.y) * 0.5f,
                              (minCorner.z + maxCorner.z) * 0.5f);

        Vector3 extents = (maxCorner - minCorner) * 0.5f;
        float biggest = std::max({extents.x, extents.y, extents.z});
        sceneRadius = std::max(1.0f, biggest * 4.0f);
    }

    camera = Camera(sceneCenter, sceneRadius);
    camera.setAspectRatio(800.0f / 600.0f);

    // Load font for overlay UI
    if (font.loadFromFile("fonts/MonospaceTypewriter.ttf"))
    {
        statsText.setFont(font);
        statsText.setCharacterSize(16);
        statsText.setFillColor(sf::Color::White);
        statsText.setPosition(8.0f, 8.0f);
    }
    else
    {
        std::cerr << "[Viewer] Font tidak dapat dimuat. UI teks dinonaktifkan.\n";
    }

    // Shader for color modulation (optional)
    const std::string fragment =
        "uniform vec4 color;"
        "void main() { gl_FragColor = color; }";

    if (shader.loadFromMemory(fragment, sf::Shader::Fragment))
    {
        shaderEnabled = true;
        shader.setUniform("color", sf::Glsl::Vec4(0.0f, 0.8f, 1.0f, 1.0f));
    }
}

void Viewer::run()
{
    sf::Clock clock;
    float deltaTime = 0.016f;

    while (window.isOpen())
    {
        processEvents();
        render(deltaTime);
        deltaTime = clock.restart().asSeconds();
    }
}

void Viewer::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        if (event.type == sf::Event::Resized)
        {
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));
            camera.setAspectRatio((float)event.size.width / (float)event.size.height);
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                isDragging = true;
                lastMousePos = sf::Mouse::getPosition(window);
            }
        }

        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                isDragging = false;
            }
        }

        if (event.type == sf::Event::MouseMoved && isDragging)
        {
            sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
            float dx = (currentMousePos.x - lastMousePos.x) * 0.01f;
            float dy = (currentMousePos.y - lastMousePos.y) * 0.01f;
            // Drag mouse up => camera moves up
            camera.orbit(-dx, dy);
            lastMousePos = currentMousePos;
        }

        if (event.type == sf::Event::MouseWheelScrolled)
        {
            camera.zoom(-event.mouseWheelScroll.delta * 0.5f);
        }
    }
}

void Viewer::render(float deltaTime)
{
    window.clear(sf::Color(30, 30, 30));

    Matrix4 view = camera.getViewMatrix();
    Matrix4 proj = camera.getProjectionMatrix();
    Matrix4 mvp = proj * view;

    sf::VertexArray lines(sf::Lines);

    auto project = [&](const Vector3 &v) -> sf::Vector2f
    {
        Vector3 p = mvp.multiplyVector(v);
        sf::Vector2u sz = window.getSize();
        return sf::Vector2f((p.x + 1.0f) * 0.5f * sz.x, (1.0f - p.y) * 0.5f * sz.y);
    };

    for (int i = 0; i < (int)voxels.size(); ++i)
    {
        const AABB &voxel = voxels[i];
        Vector3 c = voxel.center;
        Vector3 h = voxel.halfExtents;
        Vector3 corners[8] = {
            c + Vector3(-h.x, -h.y, -h.z), c + Vector3(h.x, -h.y, -h.z),
            c + Vector3(h.x, h.y, -h.z), c + Vector3(-h.x, h.y, -h.z),
            c + Vector3(-h.x, -h.y, h.z), c + Vector3(h.x, -h.y, h.z),
            c + Vector3(h.x, h.y, h.z), c + Vector3(-h.x, h.y, h.z)};

        sf::Vector2f p[8];
        for (int j = 0; j < 8; ++j)
            p[j] = project(corners[j]);

        int edges[12][2] = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}};

        for (int j = 0; j < 12; ++j)
        {
            lines.append(sf::Vertex(p[edges[j][0]], sf::Color(0, 255, 255)));
            lines.append(sf::Vertex(p[edges[j][1]], sf::Color(0, 255, 255)));
        }
    }

    if (shaderEnabled)
    {
        window.draw(lines, &shader);
    }
    else
    {
        window.draw(lines);
    }

    if (font.getInfo().family != "")
    {
        float fps = deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;
        statsText.setString("FPS: " + std::to_string((int)fps) + "\n" +
                            "Voxels: " + std::to_string(voxels.size()));
        window.draw(statsText);
    }

    window.display();
}

void Viewer::render(float deltaTime)
{
    window.clear(sf::Color(30, 30, 30));

    Matrix4 view = camera.getViewMatrix();
    Matrix4 proj = camera.getProjectionMatrix();
    Matrix4 mvp = proj * view;

    sf::VertexArray lines(sf::Lines);

    auto project = [&](const Vector3 &v) -> sf::Vector2f
    {
        Vector3 p = mvp.multiplyVector(v);
        sf::Vector2u sz = window.getSize();
        return sf::Vector2f((p.x + 1.0f) * 0.5f * sz.x, (1.0f - p.y) * 0.5f * sz.y);
    };

    auto depthOf = [&](const Vector3 &v) -> float
    {
        Vector3 p = mvp.multiplyVector(v);
        return p.z;
    };

    for (int i = 0; i < (int)voxels.size(); ++i)
    {
        const AABB &voxel = voxels[i];
        if (!isVoxelInFrustum(voxel, mvp))
            continue;

        Vector3 c = voxel.center;
        Vector3 h = voxel.halfExtents;
        Vector3 corners[8] = {
            c + Vector3(-h.x, -h.y, -h.z), c + Vector3(h.x, -h.y, -h.z),
            c + Vector3(h.x, h.y, -h.z), c + Vector3(-h.x, h.y, -h.z),
            c + Vector3(-h.x, -h.y, h.z), c + Vector3(h.x, -h.y, h.z),
            c + Vector3(h.x, h.y, h.z), c + Vector3(-h.x, h.y, h.z)};

        float depth = depthOf(c);
        sf::Color baseColor = (i == selectedVoxelIndex) ? sf::Color::Yellow : sf::Color(0, 255, 255);
        float brightness = std::max(0.2f, 1.0f - (depth + 1.0f) * 0.5f);

        sf::Color drawColor((sf::Uint8)(baseColor.r * brightness), (sf::Uint8)(baseColor.g * brightness), (sf::Uint8)(baseColor.b * brightness));

        sf::Vector2f p[8];
        for (int j = 0; j < 8; ++j)
            p[j] = project(corners[j]);

        int edges[12][2] = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

        for (int j = 0; j < 12; ++j)
        {
            lines.append(sf::Vertex(p[edges[j][0]], drawColor));
            lines.append(sf::Vertex(p[edges[j][1]], drawColor));
        }

        if (shaderEnabled)
        {
            shader.setUniform("color", sf::Glsl::Vec4(drawColor.r / 255.0f, drawColor.g / 255.0f, drawColor.b / 255.0f, 1.0f));
        }
    }

    if (shaderEnabled)
    {
        window.draw(lines, &shader);
    }
    else
    {
        window.draw(lines);
    }

    if (font.getInfo().family != "")
    {
        float fps = deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;
        statsText.setString("FPS: " + std::to_string((int)fps) + "\n" +
                            "Voxels: " + std::to_string(voxels.size()) + "\n" +
                            "Selected: " + std::to_string(selectedVoxelIndex));
        window.draw(statsText);
    }

    window.display();
}
