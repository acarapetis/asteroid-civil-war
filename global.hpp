#pragma once
#include <assert.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <list>
#include <algorithm>
#include "allegro_all.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#define foreach BOOST_FOREACH
using boost::shared_ptr;
using boost::weak_ptr;

class R2;
typedef std::vector<R2> pointVec;
#include "point.hpp"
#undef DEBUG

class Error {
public:
    enum {
        Keyboard,
        Mouse,
        Display,
        Font,
        FATAL
    };
};

const double PI = 3.14159265358979323;
const double GRAVITATIONAL_CONSTANT = 1;

#define DEGREES *(PI/180)

double randFactor();
double idealVelocity(double mass, double distance);

std::string toS(double d);
const char* toCharArray(double d);

inline double taylorSqrt(double val, double pivot, double sqrtPivot);

const ALLEGRO_COLOR WHITE   = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);
const ALLEGRO_COLOR BLACK   = al_map_rgba_f(0.0, 0.0, 0.0, 1.0);
const ALLEGRO_COLOR RED     = al_map_rgba_f(1.0, 0.0, 0.0, 1.0);
const ALLEGRO_COLOR GREEN   = al_map_rgba_f(0.0, 1.0, 0.0, 1.0);
const ALLEGRO_COLOR BLUE    = al_map_rgba_f(0.0, 0.0, 1.0, 1.0);
const ALLEGRO_COLOR YELLOW  = al_map_rgba_f(1.0, 1.0, 0.0, 1.0);
const ALLEGRO_COLOR MAGENTA = al_map_rgba_f(1.0, 0.0, 1.0, 1.0);
const ALLEGRO_COLOR CYAN    = al_map_rgba_f(0.0, 1.0, 1.0, 1.0);
const ALLEGRO_COLOR ORANGE  = al_map_rgba_f(1.0, 0.5, 0.0, 1.0);

template<class T> T lerp(T a, T b, double t) {
    return a + (b-a)*t;
}

template<class T> std::list<T> lmap(std::list<T> original, boost::function<T(T)> f) {
    std::list<T> r;
    std::copy(original.begin(), original.end(), r.begin());
    std::transform(r.begin(), r.end(), f);
}
