#include "GUI.hpp"
#include "pch.h"

GUI::GUI(Scene& scene,
    const sf::VideoMode& windowSize,
    const sf::String& windowTitle)
    : scene { scene }
    , window { windowSize, windowTitle, sf::Style::Default, sf::ContextSettings { 24, 0, 0, 4, 5 } }
{
    scene.initOpenGL();
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);
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

void GUI::update()
{
    ImGui::SFML::Update(window, deltaClock.restart());

    ImGui::Begin("Camera");
    ImGui::SliderFloat("R", &scene.camera.distance, 0.5f, 2 * scene.coord_system_edge);
    ImGui::SliderAngle("theta", &scene.camera.theta, 0.0f, 360.0f);
    ImGui::SliderAngle("phi", &scene.camera.phi, 0.0f, 180.0f);
    if (ImGui::Checkbox("Perspective projection", &scene.usePerspectiveProjection))
        scene.reshapeScreen(window.getSize());
    if (ImGui::SliderFloat("FoV", &scene.fov, 10.0f, 90.0f))
        scene.reshapeScreen(window.getSize());
    ImGui::End();

    ImGui::Begin("Transformations");
    ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&scene.position), -scene.coord_system_edge, scene.coord_system_edge);
    ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&scene.scale), .0f, 2.0f);
    ImGui::SliderFloat3("Rotation", reinterpret_cast<float*>(&scene.rotation), -180.0f, 180.0f);
    ImGui::End();

    ImGui::SFML::Render(window);

    window.display();
}