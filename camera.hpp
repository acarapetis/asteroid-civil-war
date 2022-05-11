#pragma once
#include "point.hpp"

class Camera {
    bool following;
    R2* target;
    R2 screenCenter;

public:
    R2 center = R2(0, 0);
    double rotation = 0;
    double scale = 1;

    R2 getCenter() const;

    Camera(R2 center = R2(0, 0), double rotation = 0, double scale = 1);
    Camera(R2* target, double rotation = 0, double scale = 1);

    R2 s2w(R2 screenCoords) const;
    R2 w2s(R2 worldCoords) const;
    double s2w(double screenLength) const;
    double w2s(double worldLength) const;

    void follow(R2* target);
    void toggleFollowing();
    void setScreenCenter(R2 c);
};

void camera_test();
extern const Camera untransformedCamera;
