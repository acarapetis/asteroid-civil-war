#include "global.hpp"

double randFactor() {
    return double(rand())/RAND_MAX;
}

std::string toS(double d) {
    std::ostringstream o;
    o << d;
    std::string s = o.str();
    return s;
}

const char* toCharArray(double d) {
    return toS(d).c_str();
}
double idealVelocity(double mass, double distance) {
    return sqrt(GRAVITATIONAL_CONSTANT * mass / distance);
}

inline double taylorSqrt(double val, double pivot, double sqrtPivot) {
    // Estimate sqrt(val) with a 1st-order taylor expansion about pivot
    return sqrtPivot + (val - pivot) / (2*sqrtPivot);
    //return sqrt(val);
}
