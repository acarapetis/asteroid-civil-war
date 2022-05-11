#include "graphics.hpp"

#include <allegro5/allegro_primitives.h>

#include "point.hpp"

void drawPixel(const Camera& camera, double x, double y, ALLEGRO_COLOR c) {
    drawTransformedLine(camera, x, y, x, y, c, 0);
}
void drawTransformedBitmap(const Camera& camera, ALLEGRO_BITMAP* bitmap,
                           double ax, double ay, double bx, double by,
                           double xscale, double yscale, double rotation,
                           ALLEGRO_COLOR tint) {
    R2 w = camera.w2s({bx, by});
    xscale = camera.w2s(xscale);
    yscale = camera.w2s(yscale);
    rotation += camera.rotation;
    rotation *= -1;
    al_draw_tinted_scaled_rotated_bitmap(bitmap, tint, ax, ay, w.x, w.y, xscale,
                                         yscale, rotation, 0);
}
void drawTransformedFilledCircle(const Camera& camera, double x, double y,
                                 double r, ALLEGRO_COLOR c) {
    r = camera.w2s(r);
    if (r > 0.5) {
        R2 w = camera.w2s({x, y});
        al_draw_filled_circle(w.x, w.y, r, c);
    }
}
void drawTransformedCircleBorder(const Camera& camera, double x, double y,
                                 double r, ALLEGRO_COLOR c, double t) {
    R2 w = camera.w2s({x, y});
    r = camera.w2s(r);
    t = camera.w2s(t);
    al_draw_circle(w.x, w.y, r, c, t);
}
void drawTransformedLine(const Camera& camera, double sx, double sy, double ex,
                         double ey, ALLEGRO_COLOR c, double t) {
    R2 e = camera.w2s({ex, ey});
    ex = e.x;
    ey = e.y;
    R2 s = camera.w2s({sx, sy});
    sx = s.x;
    sy = s.y;
    t = camera.w2s(t);
#ifdef DEBUG
    printf("RAW: %f,%f -> %f,%f\n", ex, ey, sx, sy);
#endif
    al_draw_line(sx, sy, ex, ey, c, t);
}

void clearScreen() { al_clear_to_color(al_map_rgba_f(0, 0, 0, 1)); }
void refreshScreen() { al_flip_display(); }
