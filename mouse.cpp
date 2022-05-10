#include "mouse.hpp"

Mouse::Mouse() {
    if (!al_install_mouse())
        throw Error::Mouse;
    this->update();
    this->position = this->deltaPosition = this->scroll = this->deltaScroll =
        R2();
    for (int i = 0; i < 3; i++)
        pressed[i] = false;
}
void Mouse::update() {
    for (int i = 0; i < 3; i++)
        newstate[i] = false;
    this->deltaPosition = R2(0, 0);
    this->deltaScroll = R2(0, 0);
}
void Mouse::processEvent(ALLEGRO_EVENT& event) {
    switch (event.type) {
    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        mousedown(&event.mouse);
        break;

    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
        mouseup(&event.mouse);
        break;

    case ALLEGRO_EVENT_MOUSE_AXES:
        mouseaxes(&event.mouse);
        break;
    }
}

void Mouse::mousedown(ALLEGRO_MOUSE_EVENT* mouse) {
    pressed[mouse->button - 1] = true;
    newstate[mouse->button - 1] = true;
}

void Mouse::mouseup(ALLEGRO_MOUSE_EVENT* mouse) {
    pressed[mouse->button - 1] = false;
    newstate[mouse->button - 1] = true;
}

void Mouse::mouseaxes(ALLEGRO_MOUSE_EVENT* mouse) {
    this->deltaPosition = R2(mouse->x, mouse->y) - this->position;
    this->position = R2(mouse->x, mouse->y);
    this->deltaScroll = R2(mouse->w, mouse->z) - this->scroll;
    this->scroll = R2(mouse->w, mouse->z);
}

bool Mouse::leftClick() { return pressed[0] && newstate[0]; }
bool Mouse::rightClick() { return pressed[1] && newstate[1]; }
bool Mouse::middleClick() { return pressed[2] && newstate[2]; }
bool Mouse::leftDown() { return pressed[0]; }
bool Mouse::rightDown() { return pressed[1]; }
bool Mouse::middleDown() { return pressed[2]; }
bool Mouse::leftRelease() { return !pressed[0] && newstate[0]; }
bool Mouse::rightRelease() { return !pressed[1] && newstate[1]; }
bool Mouse::middleRelease() { return !pressed[2] && newstate[2]; }
bool Mouse::scrollUp() { return deltaScroll.y >= 0.5; }
bool Mouse::scrollDown() { return deltaScroll.y <= -0.5; }
