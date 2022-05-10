#include "camera.hpp"

void camera_test() {
    R2 p = R2(0, 0);
    R2 r = R2(100, 0);
    assert((r.rotateAndScale(PI, p, 1) - R2(-100, 0)).lengthSquared() < .01);
    assert((r.rotateAndScale(PI, p, 3) - R2(-300, 0)).lengthSquared() < .01);
    p = R2(50, 0);
    assert((r.rotateAndScale(PI, p, 1) - R2(0, 0)).lengthSquared() < .01);
    assert((r.rotateAndScale(PI, p, 3) - R2(-100, 0)).lengthSquared() < .01);
    assert(((game.screenCenter() + R2(50, 20)) - game.camera.w2s(R2(50, 20)))
               .lengthSquared() < 0.01);
    assert(R2(10, 6) / 2 == R2(5, 3));

    Camera cam(game.screenCenter(), 0, 1);
    assert(cam.s2w(R2(1337, -707)) == R2(1337, -707));
    assert(cam.w2s(R2(1337, -707)) == R2(1337, -707));
    cam.center += R2(1, 0);
    assert(cam.s2w(R2(1337, -707)) == R2(1338, -707));
    assert(cam.w2s(R2(1337, -707)) == R2(1336, -707));

    assert(R2(100, 0).reflectInPlane(R2(0, 0), 0) == R2(100, 0));
    assert(R2(100, 0).reflectInPlane(R2(50, 0), 0) == R2(100, 0));
    assert((R2(100, 0).reflectInPlane(R2(50, 0), PI / 2) - R2(0, 0)).length() <
           0.01);
    R2(0, 100).reflectInPlane(R2(0, 50), 0).print();
    assert((R2(0, 100).reflectInPlane(R2(0, 50), 0) - R2(0, 0)).length() <
           0.01);
}

Camera::Camera(R2 center, double rotation, double scale)
    : following(false), center(center), rotation(rotation), scale(scale) {}
R2 Camera::s2w(R2 screenCoords) {
    // return (screenCoords - screenCenter).rotateAndScale(-rotation,
    // getCenter()/scale, 1.0/scale);
    return getCenter() + (screenCoords - game.screenCenter()) / scale;
}
double Camera::s2w(double screenLength) { return screenLength / scale; }
R2 Camera::w2s(R2 worldCoords) {
    //::std::cout << screenCenter.x << "," << screenCenter.y << ::std::endl;
    // return screenCenter + (worldCoords -
    // getCenter()).rotateAndScale(rotation, getCenter(), scale);
    return (worldCoords - getCenter()) * scale + game.screenCenter();
}
double Camera::w2s(double worldLength) { return worldLength * scale; }
R2 Camera::getCenter() { return (this->following ? *target : center); }
void Camera::follow(R2* target) {
    this->following = true;
    this->target = target;
}
void Camera::toggleFollowing() { this->following = !this->following; }
