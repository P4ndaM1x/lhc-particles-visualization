#pragma once
#include "../include/Particles.hpp"
#include "utils.hpp"

class Scene {
public:
    Scene(Particles& particles);

    void resetCamera();
    void resetTransformations();
    void resetAnimation();
    void initOpenGL();
    void clear();
    void draw(const app::State appState);
    void reshapeScreen(const sf::Vector2u& windowSize);
    void handleKeyboardEvents();

    void setXYplaneView();
    void setXZplaneView();
    void setYZplaneView();

    // must be a positive number! it is float only for convenience
    const float coordRayLength { 4500 };

    utils::Spherical camera;
    float fov;
    sf::Vector3f position, scale, rotation;
    bool useOrthogonalView { false };

    Particles::directions_t points;
    float animationSpeed { 1 };
    int animationFPS { 120 };
    bool animationRunning { false };

    Particles& particles;

private:
    void setCamera();
    void drawPositiveRays();
    void drawNegativeRays();
    void applyTransformations();
    void drawParticles();
    void drawPoints();

    sf::Clock timer {};
};
