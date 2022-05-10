#include "point.hpp"

R2::R2(double x, double y) : x(x), y(y) {}
R2::R2() : x(0), y(0) {}

R2 R2::operator-() const { return R2(-x, -y); }

R2 R2::operator+(R2 other) const {
    return R2(this->x + other.x, this->y + other.y);
}
void R2::operator+=(R2 other) { *this = *this + other; }

R2 R2::operator-(R2 other) const {
    return R2(this->x - other.x, this->y - other.y);
}
void R2::operator-=(R2 other) { *this = *this - other; }

R2 R2::operator*(double other) const {
    return R2(this->x * other, this->y * other);
}
void R2::operator*=(double other) { *this = *this * other; }
// Element-wise product
R2 R2::operator*(R2 other) const {
    return R2(this->x * other.x, this->y * other.y);
}
void R2::operator*=(R2 other) { *this = *this * other; }

R2 R2::operator/(double other) const {
    return R2(this->x / other, this->y / other);
}
void R2::operator/=(double other) { *this = *this / other; }
R2 R2::operator/(R2 other) const {
    return R2(this->x / other.x, this->y / other.y);
}
void R2::operator/=(R2 other) { *this = *this / other; }

double R2::dot(R2 other) const { return this->x * other.x + this->y * other.y; }
double R2::cross(R2 other) const {
    return this->x * other.y - this->y * other.x;
}

R2 R2::normalise() const { return *this / this->length(); }

R2 R2::project(R2 other) const {
    return (*this / (this->length() * this->length())) * (this->dot(other));
}

bool R2::operator==(R2 other) const { return x == other.x && y == other.y; }
bool R2::inBox(R2 tl, R2 br) const {
    return (x >= tl.x && y >= tl.y && x <= br.x && y <= br.y);
}

double R2::lengthSquared() const { return x * x + y * y; }
double R2::length() const { return sqrt(this->lengthSquared()); }
double R2::angle() const { return atan2(y, x); }
R2 R2::rotate(double radians) const {
    if (radians == 0)
        return *this;
    else
        return R2(this->length() * cos(this->angle() + radians),
                  this->length() * sin(this->angle() + radians));
}
R2 R2::rotate(double radians, R2 pivot) const {
    if (fabs(radians) < 0.01)
        return *this;
    else if (fabs(radians - PI) < 0.01)
        // return *this + 2*(pivot - *this);
        return pivot * 2 - *this;
    else
        return (*this - pivot).rotate(radians) + pivot;
}
R2 R2::scale(R2 pivot, double scale) const {
    if (fabs(scale - 1) < 0.01)
        return *this;
    else
        return pivot + (*this - pivot) * scale;
}
R2 R2::rotateAndScale(double radians, R2 pivot, double scale) const {
    if (fabs(scale - 1) < 0.01)
        return this->rotate(radians, pivot);
    if (fabs(radians) < 0.01)
        return this->scale(pivot, scale);
    R2 r = *this - pivot;
    double l = r.length() * scale;
    return R2(l * cos(r.angle() + radians), l * sin(r.angle() + radians)) +
           pivot;
}

R2 polar(double radians, double mod) {
    return R2(mod * cos(radians), mod * sin(radians));
}

R2 R2::randomise() {
    return R2((2.0 * randFactor() - 1.0) * x, (2.0 * randFactor() - 1.0) * y);
}
R2 R2::coerceDelta(R2 min, R2 max) {
    R2 delta = R2(0, 0);
    if (x < min.x) {
        delta.x = min.x - x;
        x = min.x;
    }
    if (x > max.x) {
        delta.x = max.x - x;
        x = max.x;
    }
    if (y < min.y) {
        delta.y = min.y - y;
        y = min.y;
    }
    if (y > max.y) {
        delta.y = max.y - y;
        y = max.y;
    }
    return delta;
}
void R2::coerce(R2 min, R2 max) {
    if (x < min.x) {
        x = min.x;
    }
    if (x > max.x) {
        x = max.x;
    }
    if (y < min.y) {
        y = min.y;
    }
    if (y > max.y) {
        y = max.y;
    }
}
R2 R2::reflectInPlane(R2 anyPoint, double theta) {
    // Transform co-ordinate space so that plane is horizontal:
    R2 pSpace = *this;
    if (theta != 0)
        pSpace = this->rotate(-theta, anyPoint);
    // Flip over plane:
    pSpace.y = anyPoint.y * 2 - pSpace.y;
    // Transform to original space:
    return (theta == 0 ? pSpace : pSpace.rotate(theta, anyPoint));
}
void R2::print() { std::cout << x << "," << y; }
R2 operator*(double k, R2 p) { return R2(k * p.x, k * p.y); }

bool pointInPolygon(R2 point, std::list<R2> polygon) {
    int c = 0;
    R2 prev;
    for (list<R2>::const_iterator i = polygon.begin(); i != polygon.end();
         i++) {
        R2 start, end = *i;
        if (i == polygon.begin())
            start = polygon.back(); //*(polygon.end()-1);
        else
            start = prev;

        if (intersectsPositiveX(start - point, end - point))
            ++c;
        prev = end;
    }

    return (c % 2 == 1);
}

bool intersectsPositiveX(R2 p, R2 q) {
    double det = p.y - q.y;
    double s = -q.y / det;
    double t = (q.x * p.y - q.y * p.x) / det;
    return t >= 0 && s >= 0 && s <= 1;
}

R2 intersectionPoint(R2 p, R2 q) { return lerp(p, q, -p.y / (q.y - p.y)); }

bool polygonIntersection(list<R2> a, list<R2> b) {
    for (list<R2>::const_iterator i = a.begin(); i != a.end(); i++) {
        if (pointInPolygon(*i, b))
            return true;
    }
    return false;
}

bool isCloserToOrigin(const R2& a, const R2& b) {
    return a.length() < b.length();
}

list<R2> ltransform(const list<R2>& a, double radians, R2 pivot, double scale) {
    list<R2> p;
    for (list<R2>::const_iterator i = a.begin(); i != a.end(); i++) {
        p.push_back(i->rotateAndScale(radians, pivot, scale));
    }
    return p;
}

list<R2> translate(const list<R2>& a, R2 displacement) {
    list<R2> p;
    for (list<R2>::const_iterator i = a.begin(); i != a.end(); i++) {
        p.push_back(*i + displacement);
    }
    return p;
}

list<R2> scaleToRadius(const list<R2>& ps, double r) {
    R2 max = *(std::max_element(ps.begin(), ps.end(), isCloserToOrigin));
    double ratio = r / max.length();
    list<R2> p2;
    for (list<R2>::const_iterator i = ps.begin(); i != ps.end(); i++) {
        p2.push_back(*i * ratio);
    }
    return p2;
}
