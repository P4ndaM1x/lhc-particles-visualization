#pragma once
#include "utils.hpp"

class Scene {
public:
    Scene(const Particles& particles)
        : particles { particles }
    {
    }

    void initOpenGL();
    void clear();
    void draw();
    void reshapeScreen(const sf::Vector2u& windowSize);
    void handleKeyboardEvents();

    // must be a positive number! it is float only for convenience
    const float coord_system_edge { 1000 };

    utils::Spherical camera { coord_system_edge, 1.f, .2f };
    sf::Vector3f position { 0.f, 0.f, 0.f };
    sf::Vector3f scale { 1.f, 1.f, 1.f };
    sf::Vector3f rotation { 0.f, 0.f, 0.f };
    bool usePerspectiveProjection { true };
    float fov { 45.f };

private:
    void setCamera();
    void drawPositiveRays();
    void drawNegativeRays();
    void applyTransformations();
    void drawParticles();

    const Particles& particles;
};
