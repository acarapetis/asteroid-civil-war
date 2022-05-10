#pragma once
#include "allegro_all.hpp"
#include "blending.hpp"
#include "camera.hpp"
#include "game.hpp"
#include "global.hpp"

void drawPixel(double x, double y, ALLEGRO_COLOR c);
void drawTransformedBitmap(ALLEGRO_BITMAP* bitmap, double ax, double ay,
                           double bx, double by, double xscale, double yscale,
                           double rotation);
void drawTransformedFilledCircle(double x, double y, double r, ALLEGRO_COLOR c);
void drawTransformedCircleBorder(double x, double y, double r, ALLEGRO_COLOR c,
                                 double t);
void drawTransformedLine(double sx, double sy, double ex, double ey,
                         ALLEGRO_COLOR c, double t);
void refreshScreen();
void clearScreen();

extern Game game;
