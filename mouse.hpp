#pragma once
#include "global.hpp"
#include <allegro5/allegro5.h>

class Mouse {
    bool pressed[3];
    bool newstate[3];

    /*static const int MOUSE_L       = 0x01;
    static const int MOUSE_L_NEW   = 0x02;
    static const int MOUSE_LS      = (MOUSE_L | MOUSE_L_NEW);

    static const int MOUSE_R       = 0x04;
    static const int MOUSE_R_NEW   = 0x08;
    static const int MOUSE_RS      = (MOUSE_R | MOUSE_R_NEW);

    static const int MOUSE_M       = 0x10;
    static const int MOUSE_M_NEW   = 0x20;
    static const int MOUSE_MS      = (MOUSE_M | MOUSE_M_NEW);*/

    void mousedown(struct ALLEGRO_MOUSE_EVENT *mouse);
    void mouseup(struct ALLEGRO_MOUSE_EVENT *mouse);
    void mouseaxes(struct ALLEGRO_MOUSE_EVENT *mouse);

public:
    R2 position;
    R2 deltaPosition;

    R2 scroll;
    R2 deltaScroll;

    Mouse();
    void update();

    bool leftClick();
    bool rightClick();
    bool middleClick();

    bool leftDown();
    bool rightDown();
    bool middleDown();

    bool leftRelease();
    bool rightRelease();
    bool middleRelease();

    bool scrollUp();
    bool scrollDown();

    void processEvent(ALLEGRO_EVENT& event);
};
