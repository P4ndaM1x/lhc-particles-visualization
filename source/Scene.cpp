#include "../pch.h"

#include "../include/Scene.hpp"

Scene::Scene(Particles& particles)
    : particles { particles }
{
    resetCamera();
    resetTransformations();
    resetAnimation();
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

void Scene::resetAnimation()
{
    for (const auto& directs : particles.getAllDirections()) {
        points[directs.first] = sf::Vector3f { 0, 0, 0 };
    }
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
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    for (auto& pair : particles.getFilteredSpacepoints()) {
        auto& point = pair.second;
        glVertex3f(point.x, point.y, point.z);
    }
    glEnd();

    // glEnableClientState(GL_VERTEX_ARRAY);
    // std::vector<sf::Vector3f> points;
    // for (auto& pair : particles.getAllSpacepoints()) {
    //     points.push_back(pair.second);
    // }
    // glVertexPointer(3, GL_FLOAT, sizeof(sf::Vector3f), &points[0].x);
    // glDrawArrays(GL_POINTS, 0, points.size());
    // glDisableClientState(GL_VERTEX_ARRAY);

    // std::vector<unsigned> eventIDs;
    // eventIDs.reserve(particles.getAllProperties().size());
    // for (auto map : particles.getAllProperties()) {
    //     eventIDs.push_back(map.first);
    // }

    // glColor3f(0.5, 0.5, 0.5);

    // for (unsigned id : { 1, 2, 3, 4 }) {
    //     auto& temp = particles.getFilteredSpacepoints(std::vector<unsigned> { id });
    //     glBegin(GL_LINE_STRIP);
    //     for (auto& pair : temp) {
    //         // if (pair.first != id)
    //         //     continue;
    //         // std::cout << "id: " << id << " size: " << temp.size() << std::endl;
    //         auto& point = pair.second;
    //         // std::cout << "x:" << point.x << std::endl;
    //         glVertex3f(point.x, point.y, point.z);
    //     }
    //     glEnd();
    // }
}

void Scene::drawPoints()
{
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    for (auto& pt : points) {
        const auto& props = particles.getAllProperties().at(pt.first);
        const auto& filters = particles.filters;

        bool isPhiInRange = props.phi >= filters.minPropertyValue.phi and props.phi <= filters.maxPropertyValue.phi;
        bool isPtInRange = props.pt >= filters.minPropertyValue.pt and props.pt <= filters.maxPropertyValue.pt;
        bool isQinRange = props.q >= filters.minPropertyValue.q and props.q <= filters.maxPropertyValue.q;

        if (isPhiInRange and isPtInRange and isQinRange) {
            const auto& coords = pt.second;
            glVertex3f(coords.x, coords.y, coords.z);
        }
    }
    glEnd();

    if (not animationRunning)
        return;

    const auto frequency = 1.f / animationFPS;
    if (timer.getElapsedTime().asSeconds() >= frequency) {
        const auto& directions = particles.getAllDirections();
        for (auto& pt : points) {
            const auto& eventID = pt.first;
            auto& coords = pt.second;
            coords.x -= frequency * animationSpeed * directions.at(eventID).x;
            coords.y -= frequency * animationSpeed * directions.at(eventID).y;
            coords.z -= frequency * animationSpeed * directions.at(eventID).z;
        }
        timer.restart();
    }
}

void Scene::draw(const app::State appState)
{
    clear();
    setCamera();
    drawPositiveRays();
    drawNegativeRays();
    applyTransformations();
    switch (appState) {
    case app::State::EXPLORER:
        drawParticles();
        break;
    case app::State::ANIMATION:
        drawPoints();
        break;
    default:
        break;
    }
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
