#pragma once
#include <allegro5/allegro.h>

#include "allegro_wrapper.hpp"

class Keyboard {
    char key[256];  // 256 bytes is nothing to worry about
    static const char KEYPRESSED = 0x01;
    static const char KEYNEW = 0x02;
    static const char KEYREPEAT = 0x04;

    void keydown(ALLEGRO_KEYBOARD_EVENT* kb);
    void keyup(ALLEGRO_KEYBOARD_EVENT* kb);
    void keyrepeat(ALLEGRO_KEYBOARD_EVENT* kb);

public:
    Keyboard(EventQueue& queue);
    void update();
    void clear();

    void processEvent(ALLEGRO_EVENT& event);

    bool pressed(char keycode);
    bool repeating(char keycode);
    bool isnew(char keycode);
    bool newpress(char keycode);

    operator ALLEGRO_EVENT_SOURCE*() const;
};
