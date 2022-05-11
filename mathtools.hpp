#pragma once
#include <cmath>
// header-only math utils

template <class T>
inline T lerp(T a, T b, double t) {
    return (1 - t) * a + t * b;
}

constexpr double PI = 3.14159265358979323;
constexpr double GRAVITATIONAL_CONSTANT = 1;

inline double randFactor() { return double(rand()) / RAND_MAX; }

#define DEGREES *(PI / 180)

inline double idealVelocity(double mass, double distance) {
    return sqrt(GRAVITATIONAL_CONSTANT * mass / distance);
}

inline double taylorSqrt(double val, double pivot, double sqrtPivot) {
    // Estimate sqrt(val) with a 1st-order taylor expansion about pivot
    return sqrtPivot + (val - pivot) / (2 * sqrtPivot);
    // return sqrt(val);
}
