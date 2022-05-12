#include <allegro5/allegro_font.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>

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

using std::any_of;
using std::cout;
using std::endl;
using std::list;
using std::make_shared;
using std::max;
using std::pair;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

double ddt(R2 p) { return 1; }

class Ship : public Instance {
    double currentThrustForce = 0;

public:
    R2 velocity;
    EmitterInstance emitter;

    Ship(std::shared_ptr<Drawable> visual, EmitterType et, R2 center = R2(0, 0),
         R2 velocity = R2(0, 0), double scale = 1, double rotation = 0)
        : Instance(visual, center, rotation, scale),
          velocity(velocity),
          emitter(et, center, 0, R2(), true) {}

    void thrust(double force, double dt) {
        velocity += R2(force * dt, 0).rotate(rotation);
        currentThrustForce = force;
    }
    void tick(double dt, ParticleSystem& ps) {
        center += velocity * dt;
        emitter.position = center - R2(16, 0).rotate(rotation);
        emitter.rotation = (rotation + PI);
        emitter.velocity = velocity - polar(rotation, 30);

        double thrustmod = max(currentThrustForce / 20, 1.0);
        ps.tickEmitter(emitter, dt * thrustmod);
        currentThrustForce = 0;
    }
    void draw(const Camera& camera, ALLEGRO_COLOR tint = WHITE) const {
        visual->draw(center, rotation, scale, camera, tint);
    }
    Asteroid shoot() const {
        return Asteroid(10, center + polar(rotation, 32), rotation, 1,
                        randFactor() * 10 - 5, velocity + polar(rotation, 100));
    }
};

Game game;
std::vector<Asteroid> asteroids;
ParticleType gsmoke{
    .initialVelocity = {30, 0},
    .randomVelocity = {20, 20},
    .drag = 0.9,
    .deltaScale = 0.5,
    .visual = make_shared<DynamicDrawable>(
        // 3-sided "asteroids" are nice little wonky triangles
        std::bind(&generateAsteroidPolygon, 5, 0.5, M_PI * 2 / 3, WHITE)),
    .initialTint = {1.0, 1.0, 0.0, 1.0},
    .finalTint = {1.0, 0.0, 0.0, 0.0},
    .zOrder = 1,
};
Ship ship(make_shared<Polygon>(
              list<R2>{{10, 0}, {-16, 10}, {-10, 0}, {-16, -10}}, WHITE),
          EmitterType(make_shared<ParticleType>(gsmoke), 10));

// {{{ logic update
bool update(double dt) {
#ifdef DEBUG
    printf("CAMERA scale=%f rotation=%f\n", game.camera.scale,
           game.camera.rotation);
    printf("MOUSE position=%f,%f\n", game.mouse.position.x,
           game.mouse.position.y);
#endif

    // Find colliding pairs of asteroids
    vector<pair<Asteroid, Asteroid>> collisions;
    for (auto i = asteroids.begin(); i != asteroids.end(); i++) {
        for (auto j = i + 1; j != asteroids.end(); j++) {
            if (i->isColliding(*j)) {
                collisions.push_back({std::move(*i), std::move(*j)});
                // j is further along, so we must erase it first to avoid
                // invalidating i
                j = asteroids.erase(j) - 1;
                i = asteroids.erase(i) - 1;
                break;
            }
        }
    }

    // Explode each collision into some smaller asteroids
    vector<Asteroid> toAdd;
    for (const auto& [a1, a2] : collisions) {
        for (int k = 0; k < 6; k++) {
            const Asteroid& o = (k < 3 ? a1 : a2);
            const Asteroid& u = (k < 3 ? a2 : a1);
            if (o.radius < SMALLEST_ASTEROID) continue;

            Asteroid a(o.radius / 4);
            do {
                a.center =
                    o.center + R2(o.radius / 2, o.radius / 2).randomise();
            } while (any_of(toAdd.begin(), toAdd.end(),
                            [&](const auto& b) { return a.isColliding(b); }));

            a.omega = o.omega;
            // Average velocity of the two asteroids plus some outwards vroom
            a.velocity =
                (a1.velocity + a2.velocity) / 2 + (a.center - u.center) / 4;

            toAdd.push_back(a);
        }
    }

    std::copy(toAdd.begin(), toAdd.end(), std::back_inserter(asteroids));

    for (auto& asteroid : asteroids) {
        asteroid.tick(dt * ddt(asteroid.center));
    }
    ship.tick(dt, game.ps);

    // Handle input
    if (game.mouse.scrollUp()) game.camera.scale *= 1.1;
    if (game.mouse.scrollDown()) game.camera.scale /= 1.1;
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
        asteroids.emplace_back(ship.shoot());
    }
    if (game.kb.pressed(ALLEGRO_KEY_UP)) {
        if (game.kb.pressed(ALLEGRO_KEY_TAB))
            ship.thrust(500.0, dt);
        else
            ship.thrust(100.0, dt);
    }
    if (game.kb.pressed(ALLEGRO_KEY_LEFT))
        ship.rotation -= SHIP_ROTATION_SPEED * dt;
    if (game.kb.pressed(ALLEGRO_KEY_RIGHT))
        ship.rotation += SHIP_ROTATION_SPEED * dt;
    if (game.kb.pressed(ALLEGRO_KEY_DELETE)) {
        asteroids.clear();
    }

    return true;
}
// }}}

// {{{ Draw a frame
bool draw(double dt) {
    for (const auto& a : asteroids) {
        game.draw(a);
    }
    game.draw(ship);

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

int main(int argc, char** argv) {
    for (int i = 0; i < 10; i++) {
        Asteroid a(randFactor() * 80 + 20, R2(500, 300).randomise(), 0, 1,
                   randFactor() * PI - PI / 2, R2(10, 10).randomise());
        asteroids.push_back(a);
    }

    game.camera.follow(&ship.center);
    game.run(&update, &draw);

    return 0;
}
