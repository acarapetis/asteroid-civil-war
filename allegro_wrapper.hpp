#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <stdexcept>
#include <string>

#include "point.hpp"

// RAII-compatible allegro wrappers
// There are some more involved ones in separate keyboard, mouse files

class AllegroInitializer {
public:
    AllegroInitializer() { al_init(); }
};

class EventQueue {
    AllegroInitializer _init;

public:
    ALLEGRO_EVENT_QUEUE* raw;

    EventQueue() : raw(al_create_event_queue()) {
        if (!raw) throw std::runtime_error("Could not initialize queue");
    }

    inline void registerSource(ALLEGRO_EVENT_SOURCE* source) {
        al_register_event_source(raw, source);
    }

    inline bool isEmpty() const { return al_event_queue_is_empty(raw); }

    inline ALLEGRO_EVENT next() const {
        ALLEGRO_EVENT e;
        al_wait_for_event(raw, &e);
        return e;
    }
};

class Display {
public:
    ALLEGRO_DISPLAY* raw;
    R2 size;

    Display(EventQueue& queue, R2 size = {800, 480}, bool resizeable = true) {
        if (resizeable) al_set_new_display_flags(ALLEGRO_RESIZABLE);

        al_init_primitives_addon();
        al_init_image_addon();
        al_init_font_addon();
        al_init_ttf_addon();

        raw = al_create_display(size.x, size.y);
        if (!raw) throw std::runtime_error("Could not initialize display");

        queue.registerSource(*this);
    }

    inline void handleResize() {
        size = R2(al_get_display_width(raw), al_get_display_height(raw));
    }

    inline operator ALLEGRO_DISPLAY*() const { return raw; }
    inline operator ALLEGRO_EVENT_SOURCE*() const {
        return (ALLEGRO_EVENT_SOURCE*)raw;
    }
};

class Timer {
public:
    ALLEGRO_TIMER* raw;

    Timer(EventQueue& queue) : raw(al_create_timer(0.01)) {
        if (!raw) throw std::runtime_error("Could not initialize timer");
        al_start_timer(raw);
        queue.registerSource(*this);
    }

    inline operator ALLEGRO_TIMER*() const { return raw; }
    inline operator ALLEGRO_EVENT_SOURCE*() const {
        return (ALLEGRO_EVENT_SOURCE*)raw;
    }
};

class Font {
    ALLEGRO_FONT* raw = nullptr;

public:
    Font(std::string filename, int size, int flags)
        : raw(al_load_ttf_font(filename.c_str(), size, flags)) {}
    inline operator ALLEGRO_FONT* const() { return raw; }
};
