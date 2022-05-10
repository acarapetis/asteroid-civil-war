#pragma once
#include "allegro_all.hpp"
#include "global.hpp"

class BlendMode {
public:
    int sourceMode, destMode;
    ALLEGRO_COLOR color;

    BlendMode(int sourceMode, int destMode);
    BlendMode(int sourceMode, int destMode, ALLEGRO_COLOR color);

    void apply() const;

    BlendMode interpolateTo(BlendMode dest, double coeff) const;
};

extern const BlendMode additiveBlender;
extern const BlendMode alphaBlender;
extern const BlendMode blitBlender;

BlendMode tintBlender(ALLEGRO_COLOR color);
