#include "pch.h"

int main()
{
    bool running = true;

    Particles particles { "D:\\VS22\\cvnn\\resources\\spacepoints.csv", "D:\\VS22\\cvnn\\resources\\particles_initial.csv" };
    Scene scene { particles };
    GUI gui { scene };

    while (running) {
        gui.handleWindowEvents(running);
        scene.handleKeyboardEvents();
        scene.draw();
        gui.update();
    }

    return 0;
}
