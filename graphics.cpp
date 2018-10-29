#include "graphics.hpp"

void drawPixel(double x, double y, ALLEGRO_COLOR c) {
    drawTransformedLine(x, y, x, y, c, 0);
//R2 w = game.camera.w2s(R2(x, y));
//al_draw_pixel(int(w.x), int(w.y), c);
}
void drawTransformedBitmap(ALLEGRO_BITMAP* bitmap, double ax, double ay, double bx, double by, double xscale, double yscale, double rotation) {
    R2 w = game.camera.w2s(R2(bx, by));
    bx = w.x; by = w.y;
    xscale = game.camera.w2s(xscale);
    yscale = game.camera.w2s(yscale);
    rotation += game.camera.rotation;
    rotation *= -1;
    al_draw_rotated_scaled_bitmap(bitmap, ax, ay, bx, by,
            xscale, yscale, rotation, 0);
    // this optimisation did bugger-all: 
    // seems that either allegro does plenty by itself or gcc
    // is smarter than I thought.
    /*
    if (fabs(rotation) < 0.01) {
        if (fabs(xscale - 1.0) < 0.01 && fabs(yscale - 1.0) < 0.01) {
            // draw raw
            al_draw_bitmap(bitmap, bx - ax, by - ay, 0);
            //al_draw_rotated_scaled_bitmap(bitmap, ax, ay, bx, by,
                    //xscale, yscale, rotation, 0);
        } else {
            // draw expensively: there was no advantage
            al_draw_rotated_scaled_bitmap(bitmap, ax, ay, bx, by,
                    xscale, yscale, rotation, 0);
        }
    } else {
        if (fabs(xscale - 1) < 0.01 && fabs(yscale - 1) < 0.01) {
            // draw rotated, not scaled
            al_draw_rotated_bitmap(bitmap, ax, ay, bx, by,
                    rotation, 0);
        } else {
            // draw expensively
            al_draw_rotated_scaled_bitmap(bitmap, ax, ay, bx, by,
                    xscale, yscale, rotation, 0);
        }
    }
    */
}
void drawTransformedFilledCircle(double x, double y, double r, ALLEGRO_COLOR c) {
    r = game.camera.w2s(r);
    if (r > 0.5) {
        R2 w = game.camera.w2s(R2(x, y));
        x = w.x; y = w.y;
        al_draw_filled_circle(x,y,r,c);
    }
}
void drawTransformedCircleBorder(double x, double y, double r, ALLEGRO_COLOR c, double t) {
    R2 w = game.camera.w2s(R2(x, y));
    x = w.x; y = w.y;
    r = game.camera.w2s(r);
    t = game.camera.w2s(t);
    al_draw_circle(x,y,r,c,t);
}
void drawTransformedLine(double sx, double sy, double ex, double ey, ALLEGRO_COLOR c, double t) {
    R2 e = game.camera.w2s(R2(ex, ey));
    ex = e.x; ey = e.y;
    R2 s = game.camera.w2s(R2(sx, sy));
    sx = s.x; sy = s.y;
    t = game.camera.w2s(t);
#ifdef DEBUG
    printf("RAW: %f,%f -> %f,%f\n", ex, ey, sx, sy);
#endif
    al_draw_line(sx, sy, ex, ey, c, t);
}

void clearScreen() {
    al_clear_to_color(al_map_rgba_f(0,0,0,1));
}
void refreshScreen() {
    al_flip_display();
}
