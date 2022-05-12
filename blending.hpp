#pragma once
#include "colors.hpp"

struct BlendMode {
    int sourceMode = ALLEGRO_ALPHA;
    int destMode = ALLEGRO_ONE;
    ALLEGRO_COLOR color = WHITE;

    void apply() const;

    BlendMode interpolateTo(BlendMode dest, double coeff) const;
};

extern const BlendMode additiveBlender;
extern const BlendMode alphaBlender;
extern const BlendMode blitBlender;

BlendMode tintBlender(ALLEGRO_COLOR color);
