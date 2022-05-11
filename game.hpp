#pragma once

#include <functional>

#include "allegro_wrapper.hpp"
#include "camera.hpp"
#include "graphics.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"
#include "object.hpp"
#include "particles.hpp"

// This is basically just a singleton full of globally useful stuff.
// Not much of a design pattern, but it works.
class Game {
    EventQueue queue;

public:
    Display display;
    Keyboard kb;
    Mouse mouse;
    ParticleSystem ps;
    Camera camera;

    R2 screenCenter();

    Game();
    void initialize();
    void run(std::function<bool(double)> update,
             std::function<bool(double)> draw);
    void processEvents();
    void handleScreenResize();
};
