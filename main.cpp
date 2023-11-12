#include "pch.h"

#include "include/GUI.hpp"
#include "include/Particles.hpp"
#include "include/Scene.hpp"

int main()
{
    bool running = true;

    Particles particles { "D:\\VS22\\cvnn\\resources\\spacepoints.csv", "D:\\VS22\\cvnn\\resources\\particles_initial.csv" };
    Scene scene { particles };
    GUI gui { scene };

    while (running) {
        gui.handleWindowEvents(running);
        scene.handleKeyboardEvents();
        scene.draw(gui.appState);
        gui.update();
    }

    return 0;
}
