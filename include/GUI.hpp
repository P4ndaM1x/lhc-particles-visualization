#pragma once
#include "../include/Scene.hpp"

class GUI {
public:
    GUI(Scene& scene,
        const sf::VideoMode& windowSize = { 1280, 1024 },
        const sf::String& windowTitle = { "ATLAS particles" });
    ~GUI();

    void handleWindowEvents(bool& runningFlag);
    void update();

    sf::RenderWindow window;
    app::State appState { app::State::EXPLORER };

private:
    void cameraSettings();
    void transformSettings();
    void particlesSettings();
    void animationSettings();

    Scene& scene;
    sf::Clock deltaClock {};
};
