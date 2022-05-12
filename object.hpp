#pragma once
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_image.h>

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "blending.hpp"
#include "camera.hpp"
#include "colors.hpp"
#include "graphics.hpp"
#include "mouse.hpp"
#include "point.hpp"

class Drawable {  // Pure Virtual!
public:
    // Pure Virtuals:
    virtual std::shared_ptr<Drawable> freeze() const = 0;
    virtual void draw(R2 pivot = {0, 0}, double radians = 0, double scale = 1.0,
                      const Camera& camera = untransformedCamera,
                      ALLEGRO_COLOR tint = WHITE) const = 0;
    virtual std::string describe() const = 0;
    virtual ~Drawable() = default;
};

class Sprite : public Drawable {
    ALLEGRO_BITMAP* bitmap;

public:
    R2 center;
    double scale;
    double rotation;

    std::shared_ptr<Drawable> freeze() const;

    Sprite(R2 center, ALLEGRO_BITMAP* bitmap, double rotation = 0,
           double scale = 1);

    void draw(R2 pivot = {0, 0}, double radians = 0, double scale = 1.0,
              const Camera& camera = untransformedCamera,
              ALLEGRO_COLOR tint = WHITE) const;
    std::string describe() const;
};

class Circle : public Drawable {
public:
    R2 center;
    ALLEGRO_COLOR color;
    double radius;
    bool filled;
    double border;
    ALLEGRO_COLOR borderColor;

    std::shared_ptr<Drawable> freeze() const;

    Circle(R2 center, ALLEGRO_COLOR color, double radius, bool filled,
           double border, ALLEGRO_COLOR borderColor);
    Circle(R2 center, ALLEGRO_COLOR color, double radius, bool filled = true,
           double border = -1);

    void draw(R2 pivot = {0, 0}, double radians = 0, double scale = 1.0,
              const Camera& camera = untransformedCamera,
              ALLEGRO_COLOR tint = WHITE) const;
    std::string describe() const;
};

class Line : public Drawable {
public:
    R2 start;
    R2 end;
    ALLEGRO_COLOR color;
    float thickness;

    std::shared_ptr<Drawable> freeze() const;

    Line(R2 start, R2 end, ALLEGRO_COLOR color, double thickness = 0);

    void draw() const;
    void draw(R2 pivot = {0, 0}, double radians = 0, double scale = 1.0,
              const Camera& camera = untransformedCamera,
              ALLEGRO_COLOR tint = WHITE) const;
    std::string describe() const;
};

class Polygon : public Drawable {
    void updateRadius();

public:
    std::list<R2> points;

    ALLEGRO_COLOR color;
    float thickness;
    double radius;

    std::shared_ptr<Drawable> freeze() const;

    Polygon(std::list<R2> points, ALLEGRO_COLOR color, double thickness = 0);

    void addPoint(R2 p);
    void draw(R2 pivot = {0, 0}, double radians = 0, double scale = 1.0,
              const Camera& camera = untransformedCamera,
              ALLEGRO_COLOR tint = WHITE) const;

    bool isMouseInside(const Camera& camera, const Mouse& mouse, R2 pivot,
                       double radians, double scale) const;

    void flip();

    std::string describe() const;
    std::list<R2> getPoints() const;

    std::vector<Polygon> slice(R2 origin, double angle);
};

class DynamicDrawable : public Drawable {
    std::function<std::shared_ptr<Drawable>()> generator;

public:
    DynamicDrawable(std::function<std::shared_ptr<Drawable>()> generator);
    std::shared_ptr<Drawable> freeze() const;

    void draw(R2 pivot = {0, 0}, double radians = 0, double scale = 1.0,
              const Camera& camera = untransformedCamera,
              ALLEGRO_COLOR tint = WHITE) const;
    std::string describe() const;
};

class CompoundDrawable : public Drawable {
    std::vector<std::shared_ptr<Drawable>> visualComponents;

    std::vector<std::shared_ptr<Drawable>>::const_iterator begin() const;
    std::vector<std::shared_ptr<Drawable>>::const_iterator end() const;
    std::back_insert_iterator<std::vector<std::shared_ptr<Drawable>>>
    back_inserter();

public:
    CompoundDrawable();
    CompoundDrawable(std::vector<std::shared_ptr<Drawable>> visualComponents);

    void addDrawable(std::shared_ptr<Drawable> d);
    std::shared_ptr<Drawable> freeze() const;

    void draw(R2 pivot = {0, 0}, double rotation = 0, double scale = 1.0,
              const Camera& camera = untransformedCamera,
              ALLEGRO_COLOR tint = WHITE) const;
    std::string describe() const;
};

class Instance {
public:
    std::shared_ptr<Drawable> visual;
    R2 center;
    double rotation;
    double scale;

    Instance(std::shared_ptr<Drawable> visual, R2 center, double rotation = 0,
             double scale = 1);

    void draw(const Camera& camera = untransformedCamera,
              ALLEGRO_COLOR tint = WHITE) const;
};

class Gravitee {  // More of a mixin
public:
    R2 center;
    R2 velocity;
    bool obeyGravity;
    double mass;

    void gravitateTowards(R2 center, double mass);

    Gravitee(R2 center, R2 velocity, double mass, bool obeyGravity = true);
};

class GravInstance : public Gravitee {
public:
    double scale;
    std::shared_ptr<Drawable> visual;

    GravInstance(std::shared_ptr<Drawable> visual, R2 center = {0, 0},
                 R2 velocity = {0, 0}, double scale = 1,
                 bool obeyGravity = true);

    virtual void tick(double dt);
    virtual void draw(const Camera& camera = untransformedCamera,
                      ALLEGRO_COLOR tint = WHITE) const;
    virtual ~GravInstance() = default;
};
