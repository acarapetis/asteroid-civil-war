#pragma once
#include <allegro5/allegro_color.h>

#include "object.hpp"

std::list<R2> generateAsteroid(double radius, double dr, double dtheta);
std::shared_ptr<Polygon> generateAsteroidPolygon(double radius, double dr,
                                                 double dtheta,
                                                 ALLEGRO_COLOR c);

class Asteroid : public Instance {
private:
    // Ultra-sophisticated memory-leak debugging method:
    // char megawaste[10000];
    const std::list<R2>& getPolygonPoints();

public:
    double omega;
    double radius;
    R2 velocity;

    Asteroid(double radius, R2 center = R2(0, 0), double rotation = 0,
             double scale = 1, double omega = 0, R2 velocity = R2());
    void tick(double dt);

    bool isColliding(std::shared_ptr<Asteroid> other);
    bool isColliding(Asteroid* other);
};
