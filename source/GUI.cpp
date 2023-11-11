#include "../pch.h"

#include "../include/GUI.hpp"

GUI::GUI(Scene& scene,
    const sf::VideoMode& windowSize,
    const sf::String& windowTitle)
    : scene { scene }
    , window { windowSize, windowTitle, sf::Style::Default, sf::ContextSettings { 24, 0, 0, 4, 5 } }
{
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);
    scene.initOpenGL();
    scene.reshapeScreen(window.getSize());
}

GUI::~GUI()
{
    ImGui::SFML::Shutdown(window);
}

void GUI::handleWindowEvents(bool& runningFlag)
{
    sf::Event event;
    while (window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            runningFlag = false;
        if (event.type == sf::Event::Resized)
            scene.reshapeScreen(window.getSize());
    }
}

void GUI::cameraSettings()
{
    ImGui::Begin("Camera");
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * .8f);
    ImGui::SliderFloat("R", &scene.camera.distance, 0.5f, 2 * scene.coord_system_edge);
    ImGui::SliderAngle("theta", &scene.camera.theta, 0.0f, 360.0f);
    ImGui::SliderAngle("phi", &scene.camera.phi, 0.0f, 180.0f);
    if (ImGui::Checkbox("Perspective projection", &scene.usePerspectiveProjection))
        scene.reshapeScreen(window.getSize());
    if (ImGui::SliderFloat("FoV", &scene.fov, 10.0f, 90.0f))
        scene.reshapeScreen(window.getSize());
    ImGui::End();
}

void GUI::transformSettings()
{
    ImGui::Begin("Transformations");
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * .8f);
    ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&scene.position), -scene.coord_system_edge, scene.coord_system_edge);
    ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&scene.scale), .0f, 2.0f);
    ImGui::SliderFloat3("Rotation", reinterpret_cast<float*>(&scene.rotation), -180.0f, 180.0f);
    ImGui::End();
}

void GUI::particlesSettings()
{
    auto& filters = scene.particles.filters;
    const auto& stats = scene.particles.getStats();
    ImGui::Begin("Particles filters");

    // coords
    ImGui::SeparatorText("Coordinates range");
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * .25f);
    const float coordDragSpeed { 2 };

    ImGui::DragFloat("minX", &filters.minCoordValue.x, coordDragSpeed, stats.minCoordValue.x, stats.maxCoordValue.x);
    ImGui::SameLine();
    ImGui::DragFloat("minY", &filters.minCoordValue.y, coordDragSpeed, stats.minCoordValue.y, stats.maxCoordValue.y);
    ImGui::SameLine();
    ImGui::DragFloat("minZ", &filters.minCoordValue.z, 3 * coordDragSpeed, stats.minCoordValue.z, stats.maxCoordValue.z);

    ImGui::DragFloat("maxX", &filters.maxCoordValue.x, coordDragSpeed, stats.minCoordValue.x, stats.maxCoordValue.x);
    ImGui::SameLine();
    ImGui::DragFloat("maxY", &filters.maxCoordValue.y, coordDragSpeed, stats.minCoordValue.y, stats.maxCoordValue.y);
    ImGui::SameLine();
    ImGui::DragFloat("maxZ", &filters.maxCoordValue.z, 3 * coordDragSpeed, stats.minCoordValue.z, stats.maxCoordValue.z);

    // properties
    ImGui::SeparatorText("Properties range");
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * .24f);
    const float propDragSpeed { 0.01 };

    ImGui::DragFloat("minPhi", &filters.minPropertyValue.phi, 0.66 * propDragSpeed, stats.minPropertyValue.phi, stats.maxPropertyValue.phi);
    ImGui::SameLine();
    ImGui::DragFloat("minPt", &filters.minPropertyValue.pt, propDragSpeed, stats.minPropertyValue.pt, stats.maxPropertyValue.pt);
    ImGui::SameLine();
    ImGui::DragFloat("minQ", &filters.minPropertyValue.q, 0.33 * propDragSpeed, stats.minPropertyValue.q, stats.maxPropertyValue.q);

    ImGui::DragFloat("maxPhi", &filters.maxPropertyValue.phi, 0.66 * propDragSpeed, stats.minPropertyValue.phi, stats.maxPropertyValue.phi);
    ImGui::SameLine();
    ImGui::DragFloat("maxPt", &filters.maxPropertyValue.pt, propDragSpeed, stats.minPropertyValue.pt, stats.maxPropertyValue.pt);
    ImGui::SameLine();
    ImGui::DragFloat("maxQ", &filters.maxPropertyValue.q, 0.33 * propDragSpeed, stats.minPropertyValue.q, stats.maxPropertyValue.q);

    ImGui::End();
}

void GUI::update()
{
    ImGui::SFML::Update(window, deltaClock.restart());

    cameraSettings();
    transformSettings();
    particlesSettings();

    ImGui::SFML::Render(window);
    window.display();
}
