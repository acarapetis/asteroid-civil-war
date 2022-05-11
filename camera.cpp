#include "camera.hpp"

Camera::Camera(R2 center, double rotation, double scale)
    : following(false), center(center), rotation(rotation), scale(scale) {}
Camera::Camera(R2* target, double rotation, double scale)
    : following(true), target(target), rotation(rotation), scale(scale) {}

R2 Camera::s2w(R2 screenCoords) const {
    // return (screenCoords - screenCenter).rotateAndScale(-rotation,
    // getCenter()/scale, 1.0/scale);
    return getCenter() + (screenCoords - screenCenter) / scale;
}

double Camera::s2w(double screenLength) const { return screenLength / scale; }

R2 Camera::w2s(R2 worldCoords) const {
    //::std::cout << screenCenter.x << "," << screenCenter.y << ::std::endl;
    // return screenCenter + (worldCoords -
    // getCenter()).rotateAndScale(rotation, getCenter(), scale);
    return (worldCoords - getCenter()) * scale + screenCenter;
}

double Camera::w2s(double worldLength) const { return worldLength * scale; }
R2 Camera::getCenter() const { return (this->following ? *target : center); }
void Camera::follow(R2* target) {
    this->following = true;
    this->target = target;
}
void Camera::toggleFollowing() { this->following = !this->following; }
void Camera::setScreenCenter(R2 c) { this->screenCenter = c; }

const Camera untransformedCamera;
