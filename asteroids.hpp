#pragma once
#include "allegro_all.hpp"
#include "game.hpp"
#include "global.hpp"
#include "object.hpp"

std::list<R2> generateAsteroid(double radius, double dr, double dtheta);
shared_ptr<Polygon> generateAsteroidPolygon(double radius, double dr,
                                            double dtheta, ALLEGRO_COLOR c);

class Asteroid : public Instance {
    std::list<R2> points;
    // Ultra-sophisticated memory-leak debugging method:
    // char megawaste[10000];
public:
    double omega;
    double radius;
    R2 velocity;

    Asteroid(double radius, R2 center = R2(0, 0), double rotation = 0,
             double scale = 1, double omega = 0, R2 velocity = R2());
    void tick(double dt);

    bool isColliding(shared_ptr<Asteroid> other);
    bool isColliding(Asteroid* other);
    std::list<R2> getPolygon();
};

extern Game game;
