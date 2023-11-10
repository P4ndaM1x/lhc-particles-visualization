#include "pch.h"

int main()
{
    bool running = true;

    Particles particles { "spacepoints.csv", "particles_initial.csv" };
    Scene scene { particles };
    GUI gui { scene };

    scene.reshapeScreen(gui.window.getSize());

    while (running) {
        gui.handleWindowEvents(running);
        scene.handleKeyboardEvents();
        scene.draw();
        gui.update();
    }

    return 0;
}
