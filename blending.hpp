#pragma once
#include "global.hpp"
#include "allegro_all.hpp"

class BlendMode {
public:
    int sourceMode, destMode;
    ALLEGRO_COLOR color;

    BlendMode();
    BlendMode(int sourceMode, int destMode, ALLEGRO_COLOR color);

    void apply();
    void apply(double opacity);

    BlendMode interpolateTo(BlendMode dest, double coeff);
};

void setNormalBlending();
void setAlphaBlending(double opacity);
void setAdditiveBlending(double opacity);

BlendMode additiveBlender(ALLEGRO_COLOR color);
BlendMode additiveBlender(double r, double g, double b, double a);
BlendMode alphaBlender(ALLEGRO_COLOR color);
BlendMode alphaBlender(double r, double g, double b, double a);
BlendMode blitBlender();

