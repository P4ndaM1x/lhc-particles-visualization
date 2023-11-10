#include "Scene.hpp"
#include "pch.h"

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
    glVertex3f(coord_system_edge, 0, 0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, coord_system_edge, 0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, coord_system_edge);
    glEnd();
}

void Scene::drawNegativeRays()
{
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xAAAA);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(-coord_system_edge, 0, 0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, -coord_system_edge, 0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, -coord_system_edge);
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
    for (auto& pair : particles.spacepoints) {
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
    glViewport(0, 0, (GLsizei)windowSize.x, (GLsizei)windowSize.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (Scene::usePerspectiveProjection)
        gluPerspective(Scene::fov, (GLdouble)windowSize.x / (GLdouble)windowSize.y, 0.1, 100 * (GLdouble)coord_system_edge);
    else
        glOrtho(-1.245 * ((GLdouble)windowSize.x / (GLdouble)windowSize.y), 1.245 * ((GLdouble)windowSize.x / (GLdouble)windowSize.y), -1.245, 1.245, -3.0, 12.0);
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
