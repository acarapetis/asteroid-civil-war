#pragma once
#include "allegro_all.hpp"
#include "global.hpp"
#include "blending.hpp"
#include <boost/function.hpp>

class EmitterType;
class EmitterInstance;

class Drawable { // Pure Virtual!
public:
    // Pure Virtuals:
    virtual shared_ptr<Drawable> freeze() = 0;
    virtual void draw(R2 pivot=R2(0,0), double radians=0, double scale=1.0) = 0;
    virtual std::string describe() = 0;
};

class Sprite : public Drawable {
    ALLEGRO_BITMAP *bitmap;
public:
    R2 center;
    double scale;
    double rotation;

    shared_ptr<Drawable> freeze();

    Sprite(R2 center, ALLEGRO_BITMAP *bitmap, double rotation = 0, double scale = 1);

    void draw(R2 pivot=R2(0,0), double radians=0, double scale=1.0);
    std::string describe();
};

class Circle : public Drawable {
public:
    R2 center;
    ALLEGRO_COLOR color;
    double radius;
    bool filled;
    double border;
    ALLEGRO_COLOR borderColor;

    shared_ptr<Drawable> freeze();

    Circle(R2 center, ALLEGRO_COLOR color, double radius, bool filled, double border, ALLEGRO_COLOR borderColor);
    Circle(R2 center, ALLEGRO_COLOR color, double radius, bool filled = true, double border = -1);

    void draw(R2 pivot=R2(0,0), double radians=0, double scale=1.0);
    std::string describe();
};

class Line : public Drawable {
public:
    R2 start;
    R2 end;
    ALLEGRO_COLOR color;
    float thickness;

    shared_ptr<Drawable> freeze();

    Line(R2 start, R2 end, ALLEGRO_COLOR color, double thickness=0);

    void draw();
    void draw(R2 pivot=R2(0,0), double radians=0, double scale=1.0);
    std::string describe();
};

class Polygon : public Drawable {
    std::list<R2> points;

    void updateRadius();
public:
    ALLEGRO_COLOR color;
    float thickness;
    double radius;

    shared_ptr<Drawable> freeze();

    Polygon(std::list<R2> points, ALLEGRO_COLOR color, double thickness=0);

    void addPoint(R2 p);
    void draw(R2 pivot=R2(0,0), double radians=0, double scale=1.0);

    bool mouseInside(R2 pivot, double radians, double scale);

    void flip();

    std::string describe();
    std::list<R2> getPoints();

    std::vector<Polygon> slice(R2 origin, double angle);
};

class DynamicDrawable : public Drawable {
    boost::function<shared_ptr<Drawable> ()> generator;
public:
   DynamicDrawable( boost::function<shared_ptr<Drawable> ()> generator );
   shared_ptr<Drawable> freeze();

   void draw(R2 pivot=R2(0,0), double radians=0, double scale=1.0);
   std::string describe();
};

class CompoundDrawable : public Drawable {
    std::vector<shared_ptr<Drawable> > visualComponents;

    std::vector<shared_ptr<Drawable> >::iterator begin();
    std::vector<shared_ptr<Drawable> >::iterator end();
public:
    CompoundDrawable();
    CompoundDrawable(std::vector<shared_ptr<Drawable> > visualComponents);

    void addDrawable(shared_ptr<Drawable> d);
    shared_ptr<Drawable> freeze();

    void draw(R2 pivot=R2(0,0), double rotation = 0, double scale=1.0);
    std::string describe();
};

class Instance {
public:
    shared_ptr<Drawable> visual;
    R2 center;
    double rotation;
    double scale;

    Instance(shared_ptr<Drawable> visual, R2 center, double rotation = 0, double scale = 1);

    void draw();
};

class Gravitee { // More of a mixin, but C++ blows
public:
    R2 center;
    R2 velocity;
    bool obeyGravity;
    double mass;

    void gravitateTowards(R2 center, double mass);

    Gravitee(R2 center, R2 velocity, double mass, bool obeyGravity=true);
};

class GravInstance : public Gravitee {
public:
    double scale;
    shared_ptr<Drawable> visual;

    GravInstance(shared_ptr<Drawable> visual, R2 center = R2(0,0), R2 velocity = R2(0,0), double scale = 1, bool obeyGravity = true);

    virtual void tick(double dt);
    virtual void draw();
};

class Ship : public GravInstance {
public:
    Ship(shared_ptr<Drawable> visual, shared_ptr<EmitterType> et, R2 center = R2(0,0), R2 velocity = R2(0,0), double scale = 1, bool obeyGravity = false, double rotation = 0);
    double rotation;
    shared_ptr<EmitterInstance> emitter;

    void thrust(double force, double dt);
    void tick(double dt);
    void draw();
};

#include "graphics.hpp"
#include "particles.hpp"
