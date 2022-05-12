#include <allegro5/allegro_font.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>

#include "asteroids.hpp"
#include "camera.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "keyboard.hpp"
#include "mathtools.hpp"
#include "mouse.hpp"
#include "object.hpp"
#include "particles.hpp"

const double fpsSmoothing = 10;
const double SMALLEST_ASTEROID = 10;
const double SHIP_ROTATION_SPEED = 3;

using std::cout;
using std::endl;
using std::list;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

Game game;

double ddt(R2 p) { return 1; }

class Ship : public GravInstance {
public:
    Ship(std::shared_ptr<Drawable> visual, std::shared_ptr<EmitterType> et,
         R2 center = R2(0, 0), R2 velocity = R2(0, 0), double scale = 1,
         bool obeyGravity = false, double rotation = 0)
        : GravInstance(visual, center, velocity, scale, obeyGravity),
          rotation(rotation),
          emitter(make_shared<EmitterInstance>(et, center, 0, R2(), true)) {}
    double rotation;
    std::shared_ptr<EmitterInstance> emitter;

    void thrust(double force, double dt) {
        this->velocity += R2(force * dt, 0).rotate(rotation);
        game.ps.tickEmitter(this->emitter, dt * force / 20);
    }
    void tick(double dt) {
        center += velocity * dt;
        this->emitter->position = this->center - R2(16, 0).rotate(rotation);
        this->emitter->rotation = (this->rotation + PI);
        this->emitter->velocity = this->velocity - polar(this->rotation, 30);
        game.ps.tickEmitter(this->emitter, dt);
    }
    void draw(const Camera& camera, ALLEGRO_COLOR tint = WHITE) const {
        this->visual->draw(center, rotation, scale, camera, tint);
    }
};

typedef std::list<shared_ptr<Asteroid>> astV;
astV asteroids;
shared_ptr<Drawable> smoke;
shared_ptr<ParticleType> gsmoke;
shared_ptr<Ship> ship;

// {{{ logic update
bool update(double dt) {
#ifdef DEBUG
    printf("CAMERA scale=%f rotation=%f\n", game.camera.scale,
           game.camera.rotation);
    printf("MOUSE position=%f,%f\n", game.mouse.position.x,
           game.mouse.position.y);
#endif

    astV toAdd;
    for (auto i = asteroids.begin(); i != asteroids.end();) {
        bool rmi = false;
        for (auto j = i; j != asteroids.end();) {
            if (j == i) {
                j++;
                continue;
            };
            shared_ptr<Asteroid> a1(*i);
            shared_ptr<Asteroid> a2(*j);
            if (a1->isColliding(a2)) {
                for (int k = 0; k < 6; k++) {
                    shared_ptr<Asteroid> o = (k < 3 ? a1 : a2);
                    shared_ptr<Asteroid> u = (k < 3 ? a2 : a1);
                    if (o->radius < SMALLEST_ASTEROID) continue;

                    shared_ptr<Asteroid> a;
                    bool cont = true;
                    for (; cont;) {
                        cont = false;

                        a = make_shared<Asteroid>(
                            o->radius / 6,
                            o->center +
                                R2(o->radius / 2, o->radius / 2).randomise(),
                            0, 1);

                        for (shared_ptr<Asteroid> aa : toAdd) {
                            if (a->isColliding(aa)) cont = true;
                        }
                    }

                    a->omega = o->omega;
                    a->velocity = (a1->velocity + a2->velocity) / 2 +
                                  (a->center - u->center) / 5;

                    toAdd.push_back(a);
                }
                j = asteroids.erase(j);
                rmi = true;
            } else {
                j++;
            }
        }
        if (rmi) {
            i = asteroids.erase(i);
        } else {
            i++;
        }
    }

    std::copy(toAdd.begin(), toAdd.end(), std::back_inserter(asteroids));
    for (const auto& asteroid : asteroids) {
        asteroid->tick(dt * ddt(asteroid->center));
    }

    ship->tick(dt);

    if (game.mouse.scrollUp()) game.camera.scale *= 1.1;
    if (game.mouse.scrollDown()) game.camera.scale /= 1.1;
    if (game.mouse.leftClick()) std::cout << "LEFT" << std::endl;
    if (game.kb.pressed(ALLEGRO_KEY_A))
        game.camera.center.x -= game.camera.s2w(5);
    if (game.kb.pressed(ALLEGRO_KEY_D))
        game.camera.center.x += game.camera.s2w(5);
    if (game.kb.pressed(ALLEGRO_KEY_W))
        game.camera.center.y -= game.camera.s2w(5);
    if (game.kb.pressed(ALLEGRO_KEY_S))
        game.camera.center.y += game.camera.s2w(5);
    if (game.kb.pressed(ALLEGRO_KEY_ESCAPE)) return false;
    if (game.kb.newpress(ALLEGRO_KEY_SPACE)) {
        auto a = make_shared<Asteroid>(
            10, ship->center + polar(ship->rotation, 32), ship->rotation, 1);
        a->velocity = ship->velocity + polar(ship->rotation, 100);
        asteroids.push_back(a);
    }
    if (game.kb.pressed(ALLEGRO_KEY_UP)) {
        if (game.kb.pressed(ALLEGRO_KEY_TAB))
            ship->thrust(500.0, dt);
        else
            ship->thrust(100.0, dt);
    }
    if (game.kb.pressed(ALLEGRO_KEY_LEFT))
        ship->rotation -= SHIP_ROTATION_SPEED * dt;
    if (game.kb.pressed(ALLEGRO_KEY_RIGHT))
        ship->rotation += SHIP_ROTATION_SPEED * dt;
    if (game.kb.pressed(ALLEGRO_KEY_DELETE)) {
        asteroids.clear();
    }

    return true;
}
// }}}

// {{{ Draw a frame
bool draw(double dt) {
    alphaBlender.apply();

    for (shared_ptr<Asteroid> a : asteroids) {
        a->draw(game.camera);
    }

    ship->draw(game.camera);

    static double fps = 60;
    double tdt = (dt < 0.001 ? 0.1 : dt);
    fps = (1 / fpsSmoothing) * ((fpsSmoothing - 1) * fps + 1.0 / tdt);

    string fpsStr = to_string(int(fps));
    string astCountStr = to_string(asteroids.size());
    string pCountStr = to_string(game.ps.particleCount());

    static Font smallfont("font.ttf", 24, 0);

    al_draw_text(smallfont, WHITE, 0, 0, ALLEGRO_ALIGN_LEFT,
                 (fpsStr + " FPS").c_str());
    al_draw_text(smallfont, WHITE, 0, 30, ALLEGRO_ALIGN_LEFT,
                 (astCountStr + " asteroids").c_str());
    al_draw_text(smallfont, WHITE, 0, 60, ALLEGRO_ALIGN_LEFT,
                 (pCountStr + " particles").c_str());
    return true;
}
// }}}

void testPolygon() {
    list<R2> ps;
    ps.push_back(R2(20, 20));
    ps.push_back(R2(-20, 20));
    ps.push_back(R2(-20, -20));
    ps.push_back(R2(20, -20));
    /*
    ps.push_back(R2(5,5));
    ps.push_back(R2(15,10));
    ps.push_back(R2(20,-5));
    ps.push_back(R2(10,-10));
    */

    Polygon p(ps, WHITE, 1);
    vector<Polygon> pps = p.slice(R2(-18, 22), -0.2);
    pps.push_back(p);

    for (Polygon poly : pps) {
        cout << "polygon: ";
        for (R2 point : poly.points) {
            cout << "{ " << point.x << ", " << point.y << " }, ";
        }
        cout << endl;
    }

    R2 ip = intersectionPoint(R2(10, -10), R2(20, 100));
    cout << ip.x << "," << ip.y << endl;
}

int main(int argc, char** argv) {
    testPolygon();

    for (int i = 0; i < 10; i++) {
        auto a = make_shared<Asteroid>(randFactor() * 80 + 20,
                                       R2(500, 300).randomise(), 0, 1,
                                       randFactor() * PI - PI / 2);
        a->velocity = R2(10, 10).randomise();
        asteroids.push_back(a);
    }

    smoke = make_shared<DynamicDrawable>(
        std::bind(&generateAsteroidPolygon, 5, 0.5, M_PI * 2 / 3, WHITE));

    // smoke = shared_ptr<Sprite>(new Sprite(R2(), al_load_png("bigsmoke.png"),
    // 0, 0.05));

    gsmoke = make_shared<ParticleType>(smoke);

    // ALLEGRO_BITMAP *shipbmp  = al_load_png("ship.png");

    std::list<R2> shipShape;
    shipShape.push_back(R2(10, 0));
    shipShape.push_back(R2(-16, 10));
    shipShape.push_back(R2(-10, 0));
    shipShape.push_back(R2(-16, -10));
    shared_ptr<Polygon> sp = make_shared<Polygon>(shipShape, WHITE);
    ship = make_shared<Ship>(sp, make_shared<EmitterType>(gsmoke, 10));
    game.camera.follow(&ship->center);

    gsmoke->obeyGravity = true;
    gsmoke->lifetime = 2;  // Seconds
    gsmoke->zOrder = 1;
    gsmoke->acceleration = R2(0, 0);
    gsmoke->initialBlender = alphaBlender;
    gsmoke->finalBlender = alphaBlender;
    gsmoke->initialTint = al_map_rgba_f(1.0, 1.0, 0.0, 1.0);
    gsmoke->finalTint = al_map_rgba_f(1.0, 0.0, 0.0, 0.0);
    gsmoke->initialScale = 1.0;
    gsmoke->deltaScale = 0.7;
    gsmoke->drag = 1;
    gsmoke->deltaRotation = 0;
    gsmoke->initialRotation = 0;
    gsmoke->initialVelocity = R2(30, 0);
    // gsmoke->initialVelocity = R2(2,0);
    gsmoke->randomVelocity = R2(20, 20);

#ifdef DEBUG
    cout << ast->describe() << endl;
#endif

    // testIntersections();
    //    return 0;

    game.run(&update, &draw);

    return 0;
}
