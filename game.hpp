#pragma once
#include "global.hpp"

class Game;

#include "graphics.hpp"
#include "camera.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"
#include "object.hpp" 
#include "particles.hpp"
#include "allegro_all.hpp"
#include <boost/function.hpp>

// This is basically just a singleton full of globally useful stuff.
// Not much of a design pattern, but it works.
class Game {
public:
    R2      screenSize;
    bool    resizeable;
    Camera  camera;

    ALLEGRO_DISPLAY     *display;
    ALLEGRO_EVENT_QUEUE *queue;
	ALLEGRO_TIMER       *timer;
    ALLEGRO_FONT        *smallfont;

    shared_ptr<Keyboard>        kb;
    shared_ptr<Mouse>           mouse;
    shared_ptr<ParticleSystem>  ps;

    R2 screenCenter();

    Game();
    void initialize();
    void run(boost::function<bool(double)> update, boost::function<bool(double)> draw);
    void processEvents();
};

void                 initAllegro();
ALLEGRO_TIMER*       initTimer();
ALLEGRO_DISPLAY*     initDisplay();
ALLEGRO_EVENT_QUEUE* initQueue();
