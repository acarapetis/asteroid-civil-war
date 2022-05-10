#pragma once
#include "allegro_all.hpp"
#include "global.hpp"

class Camera {
    bool following;
    R2* target;

public:
    R2 center;
    double rotation;
    double scale;
    R2 getCenter();

    Camera(R2 center = R2(0, 0), double rotation = 0, double scale = 1);

    R2 s2w(R2 screenCoords);
    R2 w2s(R2 worldCoords);
    double s2w(double screenLength);
    double w2s(double worldLength);

    void follow(R2* target);
    void toggleFollowing();
};

void camera_test();

#include "game.hpp"

extern Game game;
