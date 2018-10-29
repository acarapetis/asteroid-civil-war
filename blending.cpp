#include "blending.hpp"
void setNormalBlending() {
    setAlphaBlending(1.0);
}
void setAlphaBlending(double opacity) {
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA,
                   al_map_rgba_f(1.0, 1.0, 1.0, opacity));
}
void setAdditiveBlending(double opacity) {
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_ONE,
                   al_map_rgba_f(1.0, 1.0, 1.0, opacity));
}

BlendMode::BlendMode() : sourceMode(ALLEGRO_ALPHA), destMode(ALLEGRO_INVERSE_ALPHA), color(al_map_rgba_f(1,1,1,1)) {
}
BlendMode::BlendMode(int sourceMode, int destMode, ALLEGRO_COLOR color) : sourceMode(sourceMode), destMode(destMode), color(color) {
}
void BlendMode::apply() {
    al_set_blender(ALLEGRO_ADD, sourceMode, destMode, color);
}
void BlendMode::apply(double opacity) {
    float r, g, b;
    al_unmap_rgb_f(this->color, &r, &g, &b);
    al_set_blender(ALLEGRO_ADD, sourceMode, destMode, 
                   al_map_rgba_f(r, g, b, opacity));
}
BlendMode BlendMode::interpolateTo(BlendMode dest, double coeff) {
    assert(this->sourceMode == dest.sourceMode);
    assert(this->destMode   == dest.destMode);
    float r1, g1, b1, a1;
    float r2, g2, b2, a2;
    al_unmap_rgba_f(this->color, &r1, &g1, &b1, &a1);
    al_unmap_rgba_f(dest.color,  &r2, &g2, &b2, &a2);
    return BlendMode(sourceMode, destMode, al_map_rgba_f(
        r1 + coeff*(r2 - r1), g1 + coeff*(g2 - g1),
        b1 + coeff*(b2 - b1), a1 + coeff*(a2 - a1)
    ));
}

BlendMode additiveBlender(ALLEGRO_COLOR color) {
    return BlendMode(ALLEGRO_ALPHA, ALLEGRO_ONE, color);
}
BlendMode additiveBlender(double r, double g, double b, double a) {
    return additiveBlender(al_map_rgba_f(r,g,b,a));
}
BlendMode alphaBlender(ALLEGRO_COLOR color) {
    return BlendMode(ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA, color);
}
BlendMode alphaBlender(double r, double g, double b, double a) {
    return alphaBlender(al_map_rgba_f(r,g,b,a));
}
BlendMode blitBlender() {
    return BlendMode(ALLEGRO_ONE, ALLEGRO_ZERO, al_map_rgba_f(1,1,1,1));
}

