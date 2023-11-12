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

    ImGui::SeparatorText("Position");
    ImGui::SliderFloat("r", &scene.camera.distance, 1.f, 2 * scene.coordRayLength, "%.0f");
    ImGui::SliderAngle("theta", &scene.camera.theta, 0.f, 360.f);
    ImGui::SliderAngle("phi", &scene.camera.phi, 0.f, 180.f);

    const ImVec2 buttonSize { ImGui::GetWindowWidth() * .255f, 0 };
    if (ImGui::Button("XY plane", buttonSize))
        scene.setXYplaneView();
    ImGui::SameLine();
    if (ImGui::Button("XZ plane", buttonSize))
        scene.setXZplaneView();
    ImGui::SameLine();
    if (ImGui::Button("YZ plane", buttonSize))
        scene.setYZplaneView();

    ImGui::SeparatorText("Projection");
    ImGui::BeginDisabled(scene.useOrthogonalView);
    if (ImGui::SliderFloat("FOV", &scene.fov, 10.f, 90.f), "%.0f deg")
        scene.reshapeScreen(window.getSize());
    ImGui::EndDisabled();
    if (ImGui::Checkbox("Orthogonal view", &scene.useOrthogonalView))
        scene.reshapeScreen(window.getSize());

    ImGui::End();
}

void GUI::transformSettings()
{
    ImGui::Begin("Transformations");
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * .8f);
    ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&scene.position), -scene.coordRayLength, scene.coordRayLength, "%.0f");
    ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&scene.scale), .0f, 2.f);
    ImGui::SliderFloat3("Rotation", reinterpret_cast<float*>(&scene.rotation), -180.f, 180.f, "%.0f deg");
    if (ImGui::Button("Reset", { ImGui::GetWindowWidth() * .8f, 0 }))
        scene.resetTransformations();
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

    ImGui::BeginDisabled(appState == app::State::ANIMATION);
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
    ImGui::EndDisabled();

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

    // resets
    ImGui::SeparatorText("Resets");
    const ImVec2 buttonSize { ImGui::GetWindowWidth() * .15f, 0 };

    ImGui::BeginDisabled(appState == app::State::ANIMATION);
    if (ImGui::Button("X", buttonSize))
        scene.particles.resetFilter('x');
    ImGui::SameLine();
    if (ImGui::Button("Y", buttonSize))
        scene.particles.resetFilter('y');
    ImGui::SameLine();
    if (ImGui::Button("Z", buttonSize))
        scene.particles.resetFilter('z');
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Phi", buttonSize))
        scene.particles.resetFilter('h');
    ImGui::SameLine();
    if (ImGui::Button("Pt", buttonSize))
        scene.particles.resetFilter('t');
    ImGui::SameLine();
    if (ImGui::Button("Q", buttonSize))
        scene.particles.resetFilter('q');

    ImGui::End();
}

void GUI::animationSettings()
{

    ImGui::Begin("Application control");

    ImGui::RadioButton("detection explorer", reinterpret_cast<int*>(&appState), app::State::EXPLORER);
    ImGui::SameLine();
    ImGui::RadioButton("collision animation", reinterpret_cast<int*>(&appState), app::State::ANIMATION);
    ImGui::SeparatorText("Animation options");

    ImGui::BeginDisabled(appState == app::State::EXPLORER);
    ImGui::SliderFloat("particles speed", &scene.animationSpeed, .1f, 10.f);
    ImGui::SliderInt("frames per second", &scene.animationFPS, 1, 144);

    const ImVec2 buttonSize { ImGui::GetWindowWidth() * .15f, 0 };
    const char* startStopButtonLabel = "Start";
    if (scene.animationRunning)
        startStopButtonLabel = "Stop";
    if (ImGui::Button(startStopButtonLabel, buttonSize))
        scene.animationRunning = not scene.animationRunning;
    ImGui::SameLine();
    if (ImGui::Button("Restart", buttonSize))
        scene.resetAnimation();
    ImGui::EndDisabled();

    ImGui::End();
}

void GUI::update()
{
    ImGui::SFML::Update(window, deltaClock.restart());

    cameraSettings();
    transformSettings();
    particlesSettings();
    animationSettings();

    ImGui::SFML::Render(window);
    window.display();
}
