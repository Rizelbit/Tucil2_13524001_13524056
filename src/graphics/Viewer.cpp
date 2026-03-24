#include "graphics/Viewer.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

Viewer::Viewer(const std::vector<AABB> &voxels) : voxels(voxels), isDragging(false), lightTheta(0.785f), lightPhi(0.785f)
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(800, 600), "3D Voxel Viewer", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    // Init camera, buat target di center
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

    // Load font
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
}

void Viewer::run()
{
    sf::Clock clock;
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        render(deltaTime);
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
            // Pergerakan mouse ke atas membuat kamera bergerak ke atas
            camera.orbit(-dx, dy);
            lastMousePos = currentMousePos;
        }

        if (event.type == sf::Event::MouseWheelScrolled)
        {
            camera.zoom(-event.mouseWheelScroll.delta);
        }
    }

    // Dynamic Lighting
    if (window.hasFocus())
    {
        float step = 0.05f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) lightTheta -= step;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) lightTheta += step;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) lightPhi -= step;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) lightPhi += step;

        const float margin = 0.1f;
        if (lightPhi < margin) lightPhi = margin;
        if (lightPhi > 3.14159f - margin) lightPhi = 3.14159f - margin;
    }
}

void Viewer::render(float deltaTime)
{
    window.clear(sf::Color(30, 30, 30));

    Matrix4 view = camera.getViewMatrix();
    Matrix4 proj = camera.getProjectionMatrix();
    Matrix4 mvp = proj * view;
    Vector3 camPos = camera.getPositionWorld();

    sf::Color baseColor(255, 128, 0); // Orange
    sf::Vector2u sz = window.getSize();

    // Stage 2: Flat Shading Prep (Dynamic Light)
    Vector3 lightDir = Vector3(
        sinf(lightPhi) * cosf(lightTheta),
        cosf(lightPhi),
        sinf(lightPhi) * sinf(lightTheta)
    ).normalized();

    // Stage 3: Painter's Algorithm Prep
    struct ProjectedTriangle {
        sf::Vector2f points[3];
        float avgZ;
        sf::Color color;
    };
    std::vector<ProjectedTriangle> trianglesToDraw;

    struct Face {
        int indices[4];
        Vector3 normal;
    };

    Face faces[6] = {
        {{4, 5, 6, 7}, {0, 0, 1}},  // Near (+Z)
        {{1, 0, 3, 2}, {0, 0, -1}}, // Far (-Z)
        {{0, 4, 7, 3}, {-1, 0, 0}}, // Left (-X)
        {{5, 1, 2, 6}, {1, 0, 0}},  // Right (+X)
        {{3, 2, 6, 7}, {0, 1, 0}},  // Top (+Y)
        {{0, 1, 5, 4}, {0, -1, 0}}  // Bottom (-Y)
    };

    for (const auto &voxel : voxels)
    {
        Vector3 c = voxel.center;
        Vector3 h = voxel.halfExtents;

        // Project all 8 corners once
        Vector3 corners[8] = {
            c + Vector3(-h.x, -h.y, -h.z), // 0
            c + Vector3(h.x, -h.y, -h.z),  // 1
            c + Vector3(h.x, h.y, -h.z),   // 2
            c + Vector3(-h.x, h.y, -h.z),  // 3
            c + Vector3(-h.x, -h.y, h.z),  // 4
            c + Vector3(h.x, -h.y, h.z),   // 5
            c + Vector3(h.x, h.y, h.z),    // 6
            c + Vector3(-h.x, h.y, h.z)    // 7
        };

        // Pre-compute View Space coordinates for Z-sorting
        Vector3 viewSpace[8];
        for (int i = 0; i < 8; ++i) {
            viewSpace[i] = view.multiplyVector(corners[i]);
        }

        for (int f = 0; f < 6; ++f)
        {
            const Face& face = faces[f];
            
            // Stage 1: Backface Culling
            Vector3 faceCenter = c + face.normal * (face.normal.x != 0 ? h.x : (face.normal.y != 0 ? h.y : h.z));
            Vector3 viewDir = faceCenter - camPos;

            if (face.normal.dot(viewDir) < 0)
            {
                // Stage 2: Flat Shading Intensity
                float intensity = std::max(0.2f, face.normal.dot(lightDir * -1.0f));
                sf::Color faceColor(
                    (sf::Uint8)(baseColor.r * intensity),
                    (sf::Uint8)(baseColor.g * intensity),
                    (sf::Uint8)(baseColor.b * intensity)
                );

                // Project visible corners to 2D
                sf::Vector2f p[4];
                for (int i = 0; i < 4; ++i) {
                    Vector3 v = mvp.multiplyVector(corners[face.indices[i]]);
                    p[i] = sf::Vector2f((v.x + 1.0f) * 0.5f * (float)sz.x, (1.0f - v.y) * 0.5f * (float)sz.y);
                }

                // Stage 3: Calculate Average Z for the face (Painter's Algorithm)
                float avgZ = 0.0f;
                for (int i = 0; i < 4; ++i) avgZ += viewSpace[face.indices[i]].z;
                avgZ /= 4.0f;

                // Create 2 triangles for the face
                trianglesToDraw.push_back({{p[0], p[1], p[2]}, avgZ, faceColor});
                trianglesToDraw.push_back({{p[0], p[2], p[3]}, avgZ, faceColor});
            }
        }
    }

    // Stage 3: Sort triangles from back to front (Ascending Z since Z is negative in front)
    std::sort(trianglesToDraw.begin(), trianglesToDraw.end(), [](const ProjectedTriangle& a, const ProjectedTriangle& b) {
        return a.avgZ < b.avgZ;
    });

    // Populate VertexArray and Draw
    sf::VertexArray surface(sf::Triangles);
    for (const auto& tri : trianglesToDraw) {
        surface.append(sf::Vertex(tri.points[0], tri.color));
        surface.append(sf::Vertex(tri.points[1], tri.color));
        surface.append(sf::Vertex(tri.points[2], tri.color));
    }

    // Draw the sorted triangles
    window.draw(surface);

    if (font.getInfo().family != "")
    {
        float fps = deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;
        statsText.setString("FPS: " + std::to_string((int)fps) + "\n" +
                            "Voxels: " + std::to_string(voxels.size()) + "\n" +
                            "Light (ARROWS): T=" + std::to_string((int)(lightTheta * 180 / 3.1415f)) + 
                            ", P=" + std::to_string((int)(lightPhi * 180 / 3.1415f)));
        window.draw(statsText);
    }

    window.display();
}
