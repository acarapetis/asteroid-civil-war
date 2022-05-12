#include "asteroids.hpp"

#include "mathtools.hpp"

using std::list;
using std::make_shared;
using std::shared_ptr;

list<R2> generateAsteroid(double radius, double dr = 0.3,
                          double dtheta = PI / 8) {
    // o->addDrawable(new Circle(R2(0,0), al_map_rgba_f(1.0,1.0,1.0,1.0),
    // radius, false, 1.0, al_map_rgba_f(1.0,0.5,0.0,0.8)));

    double r = radius;
    list<R2> points;

    for (double a = 0; a < 2 * PI; a += dtheta) {
        r *= 1 - dr * (2 * randFactor() - 1);
        points.push_back(polar(a, r));
    }

    points = scaleToRadius(points, radius);
    // Polygon *poly = new Polygon(points, al_map_rgba_f(1.0,1.0,1.0,1.0));
    // poly->scaleTo(radius);
    // o->addDrawable(poly);

    return points;
}
shared_ptr<Polygon> generateAsteroidPolygon(double radius, double dr,
                                            double dtheta, ALLEGRO_COLOR c) {
    return make_shared<Polygon>(generateAsteroid(radius, dr, dtheta), c);
}

Asteroid::Asteroid(double radius, R2 center, double rotation, double scale,
                   double omega, R2 velocity)
    : Instance(make_shared<Polygon>(generateAsteroid(radius), WHITE), center,
               rotation, scale),
      omega(omega),
      radius(radius),
      velocity(velocity) {}

void Asteroid::tick(double dt) {
    this->center += velocity * dt;
    this->rotation += omega * dt;
}

bool Asteroid::isColliding(const Asteroid& other) const {
    // Always check if the small one is 'inside' the large one
    // TODO: move this check to polygonIntersection
    if (this->radius > other.radius) return other.isColliding(*this);

    return  // First check bounding circles, then run the long check if needed
        ((this->center - other.center).length() <
         (this->radius + other.radius)) &&
        polygonIntersection(
            ltransform(translate(this->getPolygonPoints(), this->center),
                       this->rotation, this->center, this->scale),
            ltransform(translate(other.getPolygonPoints(), other.center),
                       other.rotation, other.center, other.scale));
}

const list<R2>& Asteroid::getPolygonPoints() const {
    // Asteroid visuals are guaranteed to be Polygons
    return std::static_pointer_cast<Polygon>(visual)->points;
}
