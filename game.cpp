#include "game.hpp"

#include <stdexcept>

#include "blending.hpp"

using std::make_shared;

Game::Game() : display(queue), kb(queue), mouse(queue) { srand(time(NULL)); }

R2 Game::screenCenter() { return display.size / 2; }
void Game::handleScreenResize() {
    this->display.handleResize();
    this->camera.setScreenCenter(screenCenter());
}

void Game::run(std::function<bool(double)> update,
               std::function<bool(double)> draw) {
    double t = al_current_time();
    double ticks = 0;
    for (;; ticks++) {
        double nt = al_current_time();
        double dt = nt - t;
        t = nt;

        mouse.update();
        kb.update();
        processEvents();
        ps.tick(dt);

        if (!update(dt)) break;

        handleScreenResize();

        clearScreen();
        alphaBlender.apply();

        if (!draw(dt)) break;
        ps.draw(camera);

        refreshScreen();
    }
}

void Game::processEvents() {
    while (!queue.isEmpty()) {
        ALLEGRO_EVENT event = queue.next();
        switch (event.type) {
            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                handleScreenResize();
                al_acknowledge_resize(event.display.source);
                break;
            default:
                break;
        }
        mouse.processEvent(event);
        kb.processEvent(event);
    }
}

void Game::draw(const Instance& d) const {
    d.draw(camera);
}
