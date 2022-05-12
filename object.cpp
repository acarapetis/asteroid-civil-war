#include "object.hpp"

#include <algorithm>
#include <cmath>

#include "strutil.hpp"

using std::list;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

// Sprite
Sprite::Sprite(R2 center, ALLEGRO_BITMAP* bitmap, double rotation, double scale)
    : bitmap(bitmap), center(center), scale(scale), rotation(rotation) {}
void Sprite::draw(R2 pivot, double radians, double scale, const Camera& camera,
                  ALLEGRO_COLOR tint) const {
    R2 nc = (this->center + pivot).rotateAndScale(radians, pivot, scale);
    R2 ic =
        R2(al_get_bitmap_width(bitmap) / 2, al_get_bitmap_height(bitmap) / 2);
    drawTransformedBitmap(camera, bitmap, ic.x, ic.y, nc.x, nc.y,
                          this->scale * scale, this->scale * scale,
                          radians + rotation, tint);
#ifdef DEBUG
    printf("SPRITE center=%i,%i scale=%f, rotation=%f\n", int(nc.x), int(nc.y),
           this->scale * scale, radians + rotation);
#endif
}
string Sprite::describe() const { return "Sprite"; }
shared_ptr<Drawable> Sprite::freeze() const {
    return make_shared<Sprite>(*this);
}

// Circle
Circle::Circle(R2 center, ALLEGRO_COLOR color, double radius, bool filled,
               double border, ALLEGRO_COLOR borderColor)
    : center(center),
      color(color),
      radius(radius),
      filled(filled),
      border(border),
      borderColor(borderColor) {}
Circle::Circle(R2 center, ALLEGRO_COLOR color, double radius, bool filled,
               double border)
    : center(center),
      color(color),
      radius(radius),
      filled(filled),
      border(border) {
    this->borderColor = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
}
void Circle::draw(R2 pivot, double radians, double scale, const Camera& camera,
                  ALLEGRO_COLOR tint) const {
    // cout << "\t\tDrawing Circle" << endl;
    //  Pivoting still matters! Revolution != Rotation.
    R2 ac = (pivot + this->center).rotateAndScale(radians, pivot, scale);
    int rad = int(this->radius * scale);
    int bor = int(this->border * scale);
    if (this->filled)
        drawTransformedFilledCircle(camera, ac.x, ac.y, rad,
                                    this->color * tint);
    // al_draw_filled_circle(ac.x, ac.y, rad, this->color);
    if (bor >= 0)
        drawTransformedCircleBorder(camera, ac.x, ac.y, rad,
                                    this->borderColor * tint, bor);
    // al_draw_circle(ac.x, ac.y, rad, this->borderColor, bor);
}
string Circle::describe() const { return "Circle"; }
shared_ptr<Drawable> Circle::freeze() const {
    return make_shared<Circle>(*this);
}

// Line
Line::Line(R2 start, R2 end, ALLEGRO_COLOR color, double thickness)
    : start(start), end(end), color(color), thickness(thickness) {}
void Line::draw(R2 pivot, double radians, double scale, const Camera& camera,
                ALLEGRO_COLOR tint) const {
    // cout << "\t\tDrawing Line" << endl;
    R2 tStart = (this->start + pivot).rotateAndScale(radians, pivot, scale);
    R2 tEnd = (this->end + pivot).rotateAndScale(radians, pivot, scale);
#ifdef DEBUG
    printf("%f, %f -> %f, %f\n", tStart.x, tStart.y, tEnd.x, tEnd.y);
#endif
    // cout   << tStart.x << "," << tStart.y << " "
    //<< tEnd.x   << "," << tEnd.y   << endl;
    drawTransformedLine(camera, tStart.x, tStart.y, tEnd.x, tEnd.y,
                        color * tint, int(thickness * scale));
    /*
    if (thickness * scale > 2) {
        al_draw_filled_circle(tStart.x, tStart.y, int(thickness * scale / 2),
    color); al_draw_filled_circle(  tEnd.x,   tEnd.y, int(thickness * scale /
    2), color);
    }
    */
}
string Line::describe() const {
    char buf[255];
    sprintf(buf, "Line, %f,%f -> %f,%f", start.x, start.y, end.x, end.y);
    return string(buf);
}
shared_ptr<Drawable> Line::freeze() const { return make_shared<Line>(*this); }

// DynamicDrawable

DynamicDrawable::DynamicDrawable(
    std::function<shared_ptr<Drawable>()> generator)
    : generator(generator) {}
shared_ptr<Drawable> DynamicDrawable::freeze() const { return generator(); }

void DynamicDrawable::draw(R2 pivot, double radians, double scale,
                           const Camera& camera, ALLEGRO_COLOR tint) const {
    (generator())->draw(pivot, radians, scale, camera, tint);
}
string DynamicDrawable::describe() const {
    return "DynamicDrawable, sample content: " + (generator())->describe();
}

// Polygon
Polygon::Polygon(list<R2> points, ALLEGRO_COLOR color, double thickness)
    : points(points), color(color), thickness(thickness) {
    updateRadius();
}
void Polygon::updateRadius() {
    this->radius =
        std::max_element(points.begin(), points.end(), isCloserToOrigin)
            ->length();
}
void Polygon::addPoint(R2 p) {
    points.push_back(p);
    updateRadius();
}
string Polygon::describe() const {
    bool first = true;
    string s = "Polygon: ";
    for (const R2& p : points) {
        if (!first) s += " -> ";
        s += p.toString();
        first = false;
    }
    return s;
}
void Polygon::draw(R2 pivot, double radians, double scale, const Camera& camera,
                   ALLEGRO_COLOR tint) const {
    R2 startPoint = points.back();
    for (const R2& endPoint : points) {
        R2 tstart = (startPoint + pivot).rotateAndScale(radians, pivot, scale);
        R2 tend = (endPoint + pivot).rotateAndScale(radians, pivot, scale);
        drawTransformedLine(camera, tstart.x, tstart.y, tend.x, tend.y,
                            color * tint, int(thickness * scale));
        startPoint = endPoint;
    }
}
void Polygon::flip() {
    for (R2& p : points) p *= -1;
}
bool Polygon::isMouseInside(const Camera& camera, const Mouse& mouse, R2 pivot,
                            double radians, double scale) const {
    list<R2> ps = ltransform(translate(points, pivot), radians, pivot, scale);
    return pointInPolygon(camera.s2w(mouse.position), ps);
}
list<R2> Polygon::getPoints() const { return points; }
shared_ptr<Drawable> Polygon::freeze() const {
    return make_shared<Polygon>(*this);
}
vector<Polygon> Polygon::slice(R2 origin, double angle) {
    // Tasty tasty megafunction
    list<R2> tpoints = ltransform(translate(points, -origin), -angle, R2(), 1);

    vector<Polygon> results;

    bool slicing = false;
    list<R2>::iterator s2, s3, y2, y3, prev;
    R2 inter1;
    for (list<R2>::iterator i = tpoints.begin(); i != tpoints.end(); i++) {
        list<R2>::iterator end = i;
        list<R2>::iterator start =
            (i == tpoints.begin() ? tpoints.end() : prev);
        if (i == tpoints.begin()) start--;

        if (intersectsPositiveX(*start, *end)) {
            if (!slicing) {
                // Start the slice
                slicing = true;
                inter1 = intersectionPoint(*start, *end);
                s2 = tpoints.insert(end, inter1);
                s3 = tpoints.insert(end, inter1);
            } else {
                // Execute the splice
                slicing = false;
                list<R2> roid;
                inter1 = intersectionPoint(*start, *end);
                y2 = tpoints.insert(end, inter1);
                y3 = tpoints.insert(end, inter1);
                roid.splice(roid.end(), tpoints, s3, y3);
                roid = translate(ltransform(roid, angle, origin, 1), origin);
                results.push_back(Polygon(roid, color, thickness));
            }
        }

        prev = end;
    }

    points = translate(ltransform(tpoints, angle, origin, 1), origin);

    return results;
}

// CompoundDrawable
vector<shared_ptr<Drawable>>::const_iterator CompoundDrawable::begin() const {
    return this->visualComponents.begin();
}
vector<shared_ptr<Drawable>>::const_iterator CompoundDrawable::end() const {
    return this->visualComponents.end();
}
std::back_insert_iterator<vector<shared_ptr<Drawable>>>
CompoundDrawable::back_inserter() {
    return std::back_inserter(this->visualComponents);
}

CompoundDrawable::CompoundDrawable() {}
CompoundDrawable::CompoundDrawable(
    vector<shared_ptr<Drawable>> visualComponents)
    : visualComponents(visualComponents){};
void CompoundDrawable::addDrawable(shared_ptr<Drawable> d) {
    visualComponents.push_back(d);
}
shared_ptr<Drawable> CompoundDrawable::freeze() const {
    auto c = make_shared<CompoundDrawable>();
    std::copy(begin(), end(), c->back_inserter());

    for (shared_ptr<Drawable> d : visualComponents) {
        c->addDrawable(d->freeze());
    }

    return c;
}
void CompoundDrawable::draw(R2 pivot, double rotation, double scale,
                            const Camera& camera, ALLEGRO_COLOR tint) const {
    for (shared_ptr<Drawable> d : visualComponents) {
        d->draw(pivot, rotation, scale, camera, tint);
    }
}
std::string CompoundDrawable::describe() const {
    string s;
    for (shared_ptr<Drawable> d : visualComponents) {
        s += d->describe() + "\n";
    }
    return s;
}

Instance::Instance(shared_ptr<Drawable> visual, R2 center, double rotation,
                   double scale)
    : visual(visual), center(center), rotation(rotation), scale(scale) {}
void Instance::draw(const Camera& camera, ALLEGRO_COLOR tint) const {
    // R2 p = game.camera.w2s(center);
    // TODO: make this more precise so we don't have things disappearing near
    // the edge.  probably need a size member, rather than scale
    // if (p.inBox(R2(-200, -200), game.screenSize + R2(200, 200)))
    this->visual->draw(center, rotation, scale, camera, tint);
}

// Gravity "mixin"

Gravitee::Gravitee(R2 center, R2 velocity, double mass, bool obeyGravity)
    : center(center),
      velocity(velocity),
      obeyGravity(obeyGravity),
      mass(mass) {}
void Gravitee::gravitateTowards(R2 center, double mass) {
    if (!this->obeyGravity) return;
    R2 del = center - this->center;
    double d2 = del.lengthSquared();
    double d = sqrt(d2);
    double force = mass / d2;
    if (force > 2) force = 2;  // Really, *really* hacky clamping
    R2 dv = (del / d) * force;
    this->velocity += dv;
}

// A physical majigger
GravInstance::GravInstance(shared_ptr<Drawable> visual, R2 center, R2 velocity,
                           double scale, bool obeyGravity)
    : Gravitee(center, velocity, 1, obeyGravity),
      scale(scale),
      visual(visual) {}
void GravInstance::tick(double dt) { center += velocity * dt; }
void GravInstance::draw(const Camera& camera, ALLEGRO_COLOR tint) const {
    this->visual->draw(center, 0, this->scale, camera, tint);
}
