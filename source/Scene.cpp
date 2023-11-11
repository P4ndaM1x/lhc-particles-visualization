#include "../pch.h"

#include "../include/Scene.hpp"

Scene::Scene(Particles& particles)
    : particles { particles }
{
    resetCamera();
    resetTransformations();
}

void Scene::resetCamera()
{
    camera = { coordRayLength, 1.f, .2f };
    fov = 45.f;
}

void Scene::resetTransformations()
{
    position = { 0.f, 0.f, 0.f };
    scale = { 1.f, 1.f, 1.f };
    rotation = { 0.f, 0.f, 0.f };
}

void Scene::initOpenGL(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
}

void Scene::clear()
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
}

void Scene::setCamera()
{
    utils::Spherical northOfCamera { camera.distance, camera.theta, camera.phi + 0.01f };
    gluLookAt(
        camera.getX(), camera.getY(), camera.getZ(),
        0.0, 0.0, 0.0,
        northOfCamera.getX(), northOfCamera.getY(), northOfCamera.getZ());
}

void Scene::drawPositiveRays()
{
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(coordRayLength, 0, 0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, coordRayLength, 0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, coordRayLength);
    glEnd();
}

void Scene::drawNegativeRays()
{
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xAAAA);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(-coordRayLength, 0, 0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, -coordRayLength, 0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, -coordRayLength);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}

void Scene::applyTransformations()
{
    glTranslatef(position.x, position.y, position.z);
    glRotatef(rotation.x, 1, 0, 0);
    glRotatef(rotation.y, 0, 1, 0);
    glRotatef(rotation.z, 0, 0, 1);
    glScalef(scale.x, scale.y, scale.z);
}

void Scene::drawParticles()
{
    glBegin(GL_POINTS);
    glColor3f(0.0, 0.0, 0.0);
    for (auto& pair : particles.getFilteredSpacepoints()) {
        auto& point = pair.second;
        glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
}

void Scene::draw()
{
    clear();
    setCamera();
    drawPositiveRays();
    drawNegativeRays();
    applyTransformations();
    drawParticles();
}

void Scene::reshapeScreen(const sf::Vector2u& windowSize)
{
    glViewport(0, 0, windowSize.x, windowSize.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const auto& aspectRatio = static_cast<GLdouble>(windowSize.x) / static_cast<GLdouble>(windowSize.y);
    if (Scene::useOrthogonalView) {
        const auto temp = 0.25 * coordRayLength;
        glOrtho(-temp * aspectRatio, temp * aspectRatio, -temp, temp,
            -5 * coordRayLength, 5 * coordRayLength);
    } else {
        gluPerspective(Scene::fov, aspectRatio, 0.1, 10 * coordRayLength);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Scene::handleKeyboardEvents()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        camera.theta -= 0.0174533f;
        if (camera.theta < 0.0f)
            camera.theta = 0.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        camera.theta += 0.0174533f;
        if (camera.theta > (float)(2.0 * std::numbers::pi))
            camera.theta = (float)(2.0 * std::numbers::pi);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        camera.phi += 0.0174533f;
        if (camera.phi > (float)std::numbers::pi)
            camera.phi = (float)std::numbers::pi;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        camera.phi -= 0.0174533f;
        if (camera.phi < 0.0f)
            camera.phi = 0.0f;
    }
}

void Scene::setXYplaneView()
{
    camera.theta = utils::degToRad(90);
    camera.phi = utils::degToRad(0);
}

void Scene::setXZplaneView()
{
    camera.theta = utils::degToRad(90);
    camera.phi = utils::degToRad(90);
}

void Scene::setYZplaneView()
{
    camera.theta = utils::degToRad(180);
    camera.phi = utils::degToRad(0);
}
