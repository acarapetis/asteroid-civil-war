#include "asteroids.hpp"

using namespace std;

list<R2> generateAsteroid(double radius, double dr, double dtheta) {
    //o->addDrawable(new Circle(R2(0,0), al_map_rgba_f(1.0,1.0,1.0,1.0), radius, false, 1.0, al_map_rgba_f(1.0,0.5,0.0,0.8)));

    double r = radius;
    list<R2> points;

    for (double a = 0; a < 2*PI; a += dtheta) {
        r *= 1 - dr*(2*randFactor()-1);
        points.push_back(polar(a,r));
    }

    points = scaleToRadius(points, radius);
    //Polygon *poly = new Polygon(points, al_map_rgba_f(1.0,1.0,1.0,1.0));
    //poly->scaleTo(radius);
    //o->addDrawable(poly);

    return points;
}
shared_ptr<Polygon> generateAsteroidPolygon(double radius, double dr, double dtheta, ALLEGRO_COLOR c) {
    shared_ptr<Polygon> p(new Polygon(generateAsteroid(radius, dr, dtheta), c));
    p->flip();
    return p;
}

Asteroid::Asteroid(double radius, R2 center, double rotation, double scale, double omega, R2 velocity) :
    Instance(shared_ptr<Polygon>(), center, rotation, scale),
    points(generateAsteroid(radius, 0.3, PI/8)),
    omega(omega), radius(radius), velocity(velocity)
{
    visual = shared_ptr<Polygon>(new Polygon(points, WHITE));
}

void Asteroid::tick(double dt) {
    this->center   += velocity * dt;
    this->rotation += omega * dt;
}

bool Asteroid::isColliding(Asteroid *other) {
    if (this->radius > other->radius)
        return other->isColliding(this);

    return // First check bounding circles, then run the long check if needed
        ((this->center - other->center).length() < (this->radius + other->radius))
        && polygonIntersection(
                ltransform(translate(this->getPolygon(), this->center), 
                    this->rotation, this->center, this->scale), 
                ltransform(translate(other->getPolygon(), other->center),
                    other->rotation, other->center, other->scale)
        );
}

bool Asteroid::isColliding(shared_ptr<Asteroid> other) {
    // Always check if the small one is 'inside' the large one
    // TODO: move this check to polygonIntersection
    if (this->radius > other->radius)
        return other->isColliding(this);

    return this->isColliding(other.get());
}

list<R2> Asteroid::getPolygon() {
    return points;
}
