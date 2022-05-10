#include "game.hpp"

#include "blending.hpp"
Game game;

void initAllegro() {
    al_init();
    // al_font_init();
}

ALLEGRO_TIMER* initTimer() {
    ALLEGRO_TIMER* t = al_create_timer(0.01);
    al_start_timer(t);
    return t;
}

ALLEGRO_DISPLAY* initDisplay() {
    if (game.resizeable)
        al_set_new_display_flags(ALLEGRO_RESIZABLE);

    al_init_primitives_addon();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_DISPLAY* display =
        al_create_display(game.screenSize.x, game.screenSize.y);
    if (!display)
        throw Error::Display;

    return display;
}

ALLEGRO_EVENT_QUEUE* initQueue() {
    ALLEGRO_EVENT_QUEUE* q = al_create_event_queue();

    al_register_event_source(
        q, (ALLEGRO_EVENT_SOURCE*)al_get_keyboard_event_source());
    al_register_event_source(
        q, (ALLEGRO_EVENT_SOURCE*)al_get_mouse_event_source());
    al_register_event_source(q, (ALLEGRO_EVENT_SOURCE*)game.display);
    al_register_event_source(q, (ALLEGRO_EVENT_SOURCE*)game.timer);

    return q;
}

Game::Game()
    : screenSize(R2(800, 480)), resizeable(true), camera(Camera(R2(), 0, 1)) {}

void Game::initialize() {
    srand(time(NULL));

    initAllegro();

    this->timer = initTimer();
    this->display = initDisplay();

    this->kb = shared_ptr<Keyboard>(new Keyboard());
    this->mouse = shared_ptr<Mouse>(new Mouse());
    this->ps = shared_ptr<ParticleSystem>(new ParticleSystem());

    this->queue = initQueue();
    this->smallfont = al_load_ttf_font("font.ttf", 24, 0);
}

R2 Game::screenCenter() { return this->screenSize / 2; }

void Game::run(std::function<bool(double)> update,
               std::function<bool(double)> draw) {
    double t = al_current_time();
    double ticks = 0;
    for (;; ticks++) {
        double nt = al_current_time();
        double dt = nt - t;
        t = nt;

        this->mouse->update();
        this->kb->update();
        this->processEvents();
        this->ps->tick(dt);

        if (!update(dt))
            break;

        this->screenSize = R2(al_get_display_width(this->display),
                              al_get_display_height(this->display));

        clearScreen();
        alphaBlender.apply();

        if (!draw(dt))
            break;
        this->ps->draw();

        refreshScreen();
    }
}

void Game::processEvents() {
    ALLEGRO_EVENT event;
    while (!al_event_queue_is_empty(this->queue)) {
        al_wait_for_event(this->queue, &event);
        switch (event.type) {
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            this->screenSize = R2(al_get_display_width(this->display),
                                  al_get_display_height(this->display));
            al_acknowledge_resize(event.display.source);
            break;
        case ALLEGRO_EVENT_TIMER:
            break;
        default:
            break;
        }
        this->mouse->processEvent(event);
        this->kb->processEvent(event);
    }
}
