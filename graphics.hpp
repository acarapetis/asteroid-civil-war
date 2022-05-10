#pragma once
#include "allegro_all.hpp"
#include "blending.hpp"
#include "camera.hpp"
#include "game.hpp"
#include "global.hpp"

void drawPixel(double x, double y, ALLEGRO_COLOR c);
void drawTransformedBitmap(ALLEGRO_BITMAP* bitmap, double ax, double ay,
                           double bx, double by, double xscale, double yscale,
                           double rotation, ALLEGRO_COLOR tint = WHITE);
void drawTransformedFilledCircle(double x, double y, double r, ALLEGRO_COLOR c);
void drawTransformedCircleBorder(double x, double y, double r, ALLEGRO_COLOR c,
                                 double t);
void drawTransformedLine(double sx, double sy, double ex, double ey,
                         ALLEGRO_COLOR c, double t);
void refreshScreen();
void clearScreen();

inline ALLEGRO_COLOR operator*(ALLEGRO_COLOR a, ALLEGRO_COLOR b) {
    return ALLEGRO_COLOR{a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a};
}
inline ALLEGRO_COLOR operator+(ALLEGRO_COLOR a, ALLEGRO_COLOR b) {
    return ALLEGRO_COLOR{a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a};
}
inline ALLEGRO_COLOR operator*(float k, ALLEGRO_COLOR c) {
    return ALLEGRO_COLOR{k * c.r, k * c.g, k * c.b, k * c.a};
}

extern Game game;
