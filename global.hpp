#pragma once
#include <assert.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "allegro_all.hpp"
using std::cout;
using std::endl;
using std::list;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::vector;

class R2;
typedef std::vector<R2> pointVec;
#include "point.hpp"
#undef DEBUG

class Error {
public:
    enum { Keyboard, Mouse, Display, Font, FATAL };
};

const double PI = 3.14159265358979323;
const double GRAVITATIONAL_CONSTANT = 1;

#define DEGREES *(PI / 180)

double randFactor();
double idealVelocity(double mass, double distance);

std::string toS(double d);
const char* toCharArray(double d);

inline double taylorSqrt(double val, double pivot, double sqrtPivot);

const ALLEGRO_COLOR WHITE = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);
const ALLEGRO_COLOR BLACK = al_map_rgba_f(0.0, 0.0, 0.0, 1.0);
const ALLEGRO_COLOR RED = al_map_rgba_f(1.0, 0.0, 0.0, 1.0);
const ALLEGRO_COLOR GREEN = al_map_rgba_f(0.0, 1.0, 0.0, 1.0);
const ALLEGRO_COLOR BLUE = al_map_rgba_f(0.0, 0.0, 1.0, 1.0);
const ALLEGRO_COLOR YELLOW = al_map_rgba_f(1.0, 1.0, 0.0, 1.0);
const ALLEGRO_COLOR MAGENTA = al_map_rgba_f(1.0, 0.0, 1.0, 1.0);
const ALLEGRO_COLOR CYAN = al_map_rgba_f(0.0, 1.0, 1.0, 1.0);
const ALLEGRO_COLOR ORANGE = al_map_rgba_f(1.0, 0.5, 0.0, 1.0);

template <class T> inline T lerp(T a, T b, double t) {
    return (1 - t) * a + t * b;
}
