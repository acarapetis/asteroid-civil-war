#pragma once
#include "global.hpp"

class R2 {
public:
    double x, y;

    R2();
    R2(double x, double y);

    R2 operator-() const;

    R2 operator+(R2) const;
    void operator+=(R2);

    R2 operator-(R2) const;
    void operator-=(R2);

    R2 operator*(R2) const;
    void operator*=(R2);
    R2 operator*(double) const;
    void operator*=(double);

    R2 operator/(R2) const;
    void operator/=(R2);
    R2 operator/(double) const;
    void operator/=(double);

    double dot(R2 other) const;
    double cross(R2 other) const;

    R2 normalise() const;

    R2 project(R2 other) const;

    bool operator==(R2) const;

    bool inBox(R2 tl, R2 br) const;

    double lengthSquared() const;
    double length() const;
    double angle() const;

    R2 rotate(double radians) const;
    R2 rotate(double radians, R2 pivot) const;
    R2 scale(R2 pivot, double scale) const;
    R2 rotateAndScale(double radians, R2 pivot, double scale) const;
    R2 randomise();
    R2 reflectInPlane(R2 anyPoint, double theta);

    R2 coerceDelta(R2 min, R2 max);
    void coerce(R2 min, R2 max);

    void print(); // FOR DEBUGGING
};
R2 operator*(double k, R2 p);

R2 polar(double radians, double mod);

bool pointInPolygon(R2 point, std::list<R2> polygon);
bool intersectsPositiveX(R2 p, R2 q);
std::list<R2> ltransform(const std::list<R2>& a, double radians, R2 pivot,
                         double scale);
R2 intersectionPoint(R2 p, R2 q);
bool polygonIntersection(std::list<R2> a, std::list<R2> b);
bool isCloserToOrigin(const R2& a, const R2& b);
std::list<R2> translate(const std::list<R2>& a, R2 displacement);

std::list<R2> scaleToRadius(const std::list<R2>& ps, double r);

struct Transformation {
    R2 pivot;
    double rotation;
    double scale;
};
