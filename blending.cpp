#include "blending.hpp"

BlendMode::BlendMode(int sourceMode, int destMode, ALLEGRO_COLOR color)
    : sourceMode(sourceMode), destMode(destMode), color(color) {}
BlendMode::BlendMode(int sourceMode, int destMode)
    : BlendMode(sourceMode, destMode, WHITE) {}
void BlendMode::apply() const {
    al_set_blender(ALLEGRO_ADD, sourceMode, destMode);
    al_set_blend_color(color);
}
BlendMode BlendMode::interpolateTo(BlendMode dest, double coeff) const {
    assert(this->sourceMode == dest.sourceMode);
    assert(this->destMode == dest.destMode);
    float r1, g1, b1, a1;
    float r2, g2, b2, a2;
    al_unmap_rgba_f(this->color, &r1, &g1, &b1, &a1);
    al_unmap_rgba_f(dest.color, &r2, &g2, &b2, &a2);
    return BlendMode(
        sourceMode, destMode,
        al_map_rgba_f(r1 + coeff * (r2 - r1), g1 + coeff * (g2 - g1),
                      b1 + coeff * (b2 - b1), a1 + coeff * (a2 - a1)));
}

const BlendMode additiveBlender(ALLEGRO_ALPHA, ALLEGRO_ONE, WHITE);
const BlendMode alphaBlender(ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA, WHITE);
const BlendMode blitBlender(ALLEGRO_ONE, ALLEGRO_ZERO, WHITE);

BlendMode tintBlender(ALLEGRO_COLOR color) {
    return BlendMode(ALLEGRO_CONST_COLOR, ALLEGRO_ZERO, color);
}
