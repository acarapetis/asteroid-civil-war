#include "keyboard.hpp"

void Keyboard::keydown(ALLEGRO_KEYBOARD_EVENT *kb) {
    key[kb->keycode] = KEYPRESSED | KEYNEW;
}
void Keyboard::keyup(ALLEGRO_KEYBOARD_EVENT *kb) {
    key[kb->keycode] &= ~KEYPRESSED;
}
void Keyboard::keyrepeat(ALLEGRO_KEYBOARD_EVENT *kb) {
    key[kb->keycode] |= KEYREPEAT;
}
Keyboard::Keyboard() {
    if (!al_install_keyboard()) throw Error::Keyboard;
    this->clear();
}
void Keyboard::update() {
    int i;
    static int val =  ((KEYNEW | KEYREPEAT) << 24) | ((KEYNEW | KEYREPEAT) << 16) | ((KEYNEW | KEYREPEAT) << 8) | KEYNEW | KEYREPEAT;

    for(i=0; i<64; i++) ((int*)key)[i] &= ~val;
}
void Keyboard::clear() {
    memset(key, 0, sizeof(*key)*256);
}
void Keyboard::processEvent(ALLEGRO_EVENT& event) {
    switch(event.type) {
        case ALLEGRO_EVENT_KEY_DOWN:
            keydown(&event.keyboard);
            break;

        case ALLEGRO_EVENT_KEY_UP:
            keyup(&event.keyboard);
            break;

        //case ALLEGRO_EVENT_KEY_REPEAT:
            //keyrepeat(&event.keyboard);
            //break;
    }
}
bool Keyboard::pressed(char keycode) {
    return (key[(int)keycode] & KEYPRESSED);
}
bool Keyboard::repeating(char keycode) {
    return (key[(int)keycode] & KEYREPEAT);
}
bool Keyboard::isnew(char keycode) {
    return (key[(int)keycode] & KEYNEW);
}
bool Keyboard::newpress(char keycode) {
    return (isnew(keycode) && pressed(keycode));
}
