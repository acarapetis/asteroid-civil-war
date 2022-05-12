#include <iostream>
#include <list>
#include <vector>

#include "camera.hpp"
#include "game.hpp"
#include "mathtools.hpp"
#include "point.hpp"

using namespace std;

Game game;

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

void testIntersections() {
    assert(intersectsPositiveX(R2(1, 1), R2(1, -1)));
    assert(intersectsPositiveX(R2(100, 1), R2(1, -1)));
    assert(intersectsPositiveX(R2(-5, 1), R2(10, -1)));
    assert(!intersectsPositiveX(R2(-10, 1), R2(5, -1)));
}

void testPolygon() {
    list<R2> ps;
    ps.push_back(R2(20, 20));
    ps.push_back(R2(-20, 20));
    ps.push_back(R2(-20, -20));
    ps.push_back(R2(20, -20));
    /*
    ps.push_back(R2(5,5));
    ps.push_back(R2(15,10));
    ps.push_back(R2(20,-5));
    ps.push_back(R2(10,-10));
    */

    Polygon p(ps, WHITE, 1);
    vector<Polygon> pps = p.slice(R2(-18, 22), -0.2);
    pps.push_back(p);

    for (Polygon poly : pps) {
        cout << "polygon: ";
        for (R2 point : poly.points) {
            cout << "{ " << point.x << ", " << point.y << " }, ";
        }
        cout << endl;
    }

    R2 ip = intersectionPoint(R2(10, -10), R2(20, 100));
    cout << ip.x << "," << ip.y << endl;
}
