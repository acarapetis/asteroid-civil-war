#include "object.hpp"

#include <boost/lambda/lambda.hpp>

// Sprite
Sprite::Sprite(R2 center, ALLEGRO_BITMAP* bitmap, double rotation, double scale)
    : bitmap(bitmap), center(center), scale(scale), rotation(rotation) {}
void Sprite::draw(R2 pivot, double radians, double scale) {
    R2 nc = (this->center + pivot).rotateAndScale(radians, pivot, scale);
    R2 ic =
        R2(al_get_bitmap_width(bitmap) / 2, al_get_bitmap_height(bitmap) / 2);
    drawTransformedBitmap(bitmap, ic.x, ic.y, nc.x, nc.y, this->scale * scale,
                          this->scale * scale, radians + rotation);
#ifdef DEBUG
    printf("SPRITE center=%i,%i scale=%f, rotation=%f\n", int(nc.x), int(nc.y),
           this->scale * scale, radians + rotation);
#endif
}
string Sprite::describe() { return "Sprite"; }
shared_ptr<Drawable> Sprite::freeze() {
    return shared_ptr<Drawable>(new Sprite(*this));
}

// Circle
Circle::Circle(R2 center, ALLEGRO_COLOR color, double radius, bool filled,
               double border, ALLEGRO_COLOR borderColor)
    : center(center), color(color), radius(radius), filled(filled),
      border(border), borderColor(borderColor) {}
Circle::Circle(R2 center, ALLEGRO_COLOR color, double radius, bool filled,
               double border)
    : center(center), color(color), radius(radius), filled(filled),
      border(border) {
    this->borderColor = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
}
void Circle::draw(R2 pivot, double radians, double scale) {
    // cout << "\t\tDrawing Circle" << endl;
    //  Pivoting still matters! Revolution != Rotation.
    R2 ac = (pivot + this->center).rotateAndScale(radians, pivot, scale);
    int rad = int(this->radius * scale);
    int bor = int(this->border * scale);
    if (this->filled)
        drawTransformedFilledCircle(ac.x, ac.y, rad, this->color);
    // al_draw_filled_circle(ac.x, ac.y, rad, this->color);
    if (bor >= 0)
        drawTransformedCircleBorder(ac.x, ac.y, rad, this->borderColor, bor);
    // al_draw_circle(ac.x, ac.y, rad, this->borderColor, bor);
}
string Circle::describe() { return "Circle"; }
shared_ptr<Drawable> Circle::freeze() {
    return shared_ptr<Drawable>(new Circle(*this));
}

// Line
Line::Line(R2 start, R2 end, ALLEGRO_COLOR color, double thickness)
    : start(start), end(end), color(color), thickness(thickness) {}
void Line::draw(R2 pivot, double radians, double scale) {
    // cout << "\t\tDrawing Line" << endl;
    R2 tStart = (this->start + pivot).rotateAndScale(radians, pivot, scale);
    R2 tEnd = (this->end + pivot).rotateAndScale(radians, pivot, scale);
#ifdef DEBUG
    printf("%f, %f -> %f, %f\n", tStart.x, tStart.y, tEnd.x, tEnd.y);
#endif
    // cout   << tStart.x << "," << tStart.y << " "
    //<< tEnd.x   << "," << tEnd.y   << endl;
    drawTransformedLine(tStart.x, tStart.y, tEnd.x, tEnd.y, color,
                        int(thickness * scale));
    /*
    if (thickness * scale > 2) {
        al_draw_filled_circle(tStart.x, tStart.y, int(thickness * scale / 2),
    color); al_draw_filled_circle(  tEnd.x,   tEnd.y, int(thickness * scale /
    2), color);
    }
    */
}
string Line::describe() {
    char buf[255];
    sprintf(buf, "Line, %f,%f -> %f,%f", start.x, start.y, end.x, end.y);
    return string(buf);
}
shared_ptr<Drawable> Line::freeze() {
    return shared_ptr<Drawable>(new Line(*this));
}

// DynamicDrawable

DynamicDrawable::DynamicDrawable(
    boost::function<shared_ptr<Drawable>()> generator)
    : generator(generator) {}
shared_ptr<Drawable> DynamicDrawable::freeze() { return generator(); }

void DynamicDrawable::draw(R2 pivot, double radians, double scale) {
    (generator())->draw(pivot, radians, scale);
}
string DynamicDrawable::describe() {
    return "DynamicDrawable, sample content: " + (generator())->describe();
}

// Polygon
Polygon::Polygon(std::list<R2> points, ALLEGRO_COLOR color, double thickness)
    : points(points), color(color), thickness(thickness) {
    updateRadius();
}
void Polygon::updateRadius() {
    this->radius =
        max_element(points.begin(), points.end(), isCloserToOrigin)->length();
}
void Polygon::addPoint(R2 p) {
    points.push_back(p);
    updateRadius();
}
string Polygon::describe() {
    string s = "Polygon: ";
    for (std::list<R2>::const_iterator i = points.begin(); i != points.end();
         i++) {
        if (i != points.begin())
            s += " -> ";
        s += lexical_cast<string>(i->x) + "," + lexical_cast<string>(i->y);
    }
    return s;
}
void Polygon::draw(R2 pivot, double radians, double scale) {
    R2 prev;
    for (std::list<R2>::const_iterator i = points.begin(); i != points.end();
         i++) {
        R2 start, end = *i;
        if (i == points.begin())
            start = points.back(); //*(points.end()-1);
        else
            start = prev;

        R2 tstart = (start + pivot).rotateAndScale(radians, pivot, scale);
        R2 tend = (end + pivot).rotateAndScale(radians, pivot, scale);
        drawTransformedLine(tstart.x, tstart.y, tend.x, tend.y, color,
                            int(thickness * scale));
        prev = end;
    }

#ifdef DEBUG
    ALLEGRO_COLOR c = mouseInside(radians, pivot, scale) ? GREEN : RED;

    drawTransformedFilledCircle(pivot.x, pivot.y, radius * scale / 4, c);
#endif
}
void Polygon::flip() {
    for (std::list<R2>::iterator i = points.begin(); i != points.end(); i++)
        *i *= -1;
}
bool Polygon::mouseInside(R2 pivot, double radians, double scale) {
    std::list<R2> ps =
        ltransform(translate(points, pivot), radians, pivot, scale);
    return pointInPolygon(game.camera.s2w(game.mouse->position), ps);
}
std::list<R2> Polygon::getPoints() { return points; }
shared_ptr<Drawable> Polygon::freeze() {
    return shared_ptr<Drawable>(new Polygon(*this));
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
        if (i == tpoints.begin())
            start--;

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
vector<shared_ptr<Drawable> >::iterator CompoundDrawable::begin() {
    return this->visualComponents.begin();
}
vector<shared_ptr<Drawable> >::iterator CompoundDrawable::end() {
    return this->visualComponents.end();
}

CompoundDrawable::CompoundDrawable() {}
CompoundDrawable::CompoundDrawable(
    vector<shared_ptr<Drawable> > visualComponents)
    : visualComponents(visualComponents){};
void CompoundDrawable::addDrawable(shared_ptr<Drawable> d) {
    visualComponents.push_back(d);
}
shared_ptr<Drawable> CompoundDrawable::freeze() {
    shared_ptr<CompoundDrawable> c(new CompoundDrawable());

    foreach (shared_ptr<Drawable> d, visualComponents) {
        c->addDrawable(d->freeze());
    }

    return c;
}
void CompoundDrawable::draw(R2 pivot, double rotation, double scale) {
    foreach (shared_ptr<Drawable> d, visualComponents) {
        d->draw(pivot, rotation, scale);
    }
}
std::string CompoundDrawable::describe() {
    string s;
    foreach (shared_ptr<Drawable> d, visualComponents) {
        s += d->describe() + "\n";
    }
    return s;
}

Instance::Instance(shared_ptr<Drawable> visual, R2 center, double rotation,
                   double scale)
    : visual(visual), center(center), rotation(rotation), scale(scale) {}
void Instance::draw() {
    R2 p = game.camera.w2s(center);
    // TODO: make this more precise so we don't have things disappearing near
    // the edge.  probably need a size member, rather than scale
    if (p.inBox(R2(-200, -200), game.screenSize + R2(200, 200)))
        this->visual->draw(center, rotation, scale);
}

// Gravity "mixin"

Gravitee::Gravitee(R2 center, R2 velocity, double mass, bool obeyGravity)
    : center(center), velocity(velocity), obeyGravity(obeyGravity), mass(mass) {
}
void Gravitee::gravitateTowards(R2 center, double mass) {
    if (!this->obeyGravity)
        return;
    R2 del = center - this->center;
    double d2 = del.lengthSquared();
    double d = sqrt(d2);
    double force = mass / d2;
    if (force > 2)
        force = 2; // Really, *really* hacky clamping
    R2 dv = (del / d) * force;
    this->velocity += dv;
}

// A physical majigger
GravInstance::GravInstance(shared_ptr<Drawable> visual, R2 center, R2 velocity,
                           double scale, bool obeyGravity)
    : Gravitee(center, velocity, 1, obeyGravity), scale(scale), visual(visual) {
}
void GravInstance::tick(double dt) { center += velocity * dt; }
void GravInstance::draw() { this->visual->draw(center, 0, this->scale); }

Ship::Ship(shared_ptr<Drawable> visual, shared_ptr<EmitterType> et, R2 center,
           R2 velocity, double scale, bool obeyGravity, double rotation)
    : GravInstance(visual, center, velocity, scale, obeyGravity),
      rotation(rotation), emitter(shared_ptr<EmitterInstance>(
                              new EmitterInstance(et, center, 0, R2(), true))) {
}
void Ship::draw() { this->visual->draw(center, rotation, scale); }
void Ship::thrust(double force, double dt) {
    this->velocity += R2(force * dt, 0).rotate(rotation);
    game.ps->tickEmitter(this->emitter, dt * force / 20);
}
void Ship::tick(double dt) {
    center += velocity * dt;
    this->emitter->position = this->center - R2(16, 0).rotate(rotation);
    this->emitter->rotation = (this->rotation + PI);
    this->emitter->velocity = this->velocity - polar(this->rotation, 30);
    game.ps->tickEmitter(this->emitter, dt);
}
