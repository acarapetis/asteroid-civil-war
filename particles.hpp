/*
 * particles.hpp
 * author: Anthony Carapetis
 * project: gravgame thingy
 *
 * definitions for the Particle System - these are for visual effects.
 * their motion is determined by the *Types, and possibly by calls to
 * applyGravity().
 */
#pragma once
#include <algorithm>
#include <map>
#include <utility>

#include "allegro_all.hpp"

// Everybody can know about everyone else.
// Bite me, Djikstra. I'm anthropomorphising my classes.
// What you going to do about it?
class ParticleInstance;
class ParticleType;
class EmitterType;
class EmitterInstance;
class ParticleSystem;

#include "blending.hpp"
#include "global.hpp"
#include "graphics.hpp"
#include "object.hpp"

// Numerically Calculated Particle - all values have a delta function
// applied each frame, additive or multiplicative based on commonsense.
class ParticleType {
public:
    shared_ptr<Drawable> visual;
    double lifetime;
    bool obeyGravity;

    R2 initialVelocity;
    R2 randomVelocity;
    R2 acceleration; // additive
    double drag;     // multiplicative

    double initialRotation;
    double randomRotation;
    double deltaRotation; // additive

    double initialScale;
    double randomScale;
    double deltaScale; // multiplicative

    BlendMode initialBlender;
    BlendMode finalBlender;

    double zOrder;

    ParticleType(shared_ptr<Drawable> visual);

    BlendMode interpolateBlender(double age);
};

class ParticleInstance : public Gravitee {
    shared_ptr<ParticleType> type;
    shared_ptr<Drawable> visual;

public:
    double age;
    double vRot;

    shared_ptr<ParticleType> getType();

    BlendMode blender();

    double zOrder() const;

    bool frozen;
    void freezeVisual();

    bool expired();
    // provided by gravitee: R2 velocity;
    double rotation;
    double scale;

    R2 getVelocity();
    double getRotation();
    double getScale();
    R2 getCenter();

    void draw();
    void tick(double dt);

    // provided by gravitee: void gravitateTowards(R2 position, double mass);
    ParticleInstance(shared_ptr<ParticleType> type, R2 center = R2(),
                     R2 velocity = R2(), double vRot = 0);
    ParticleInstance(shared_ptr<EmitterInstance> emitter);
};

struct compareZOrder {
    // Sorting relation for order of particle rendering
    // Precedence: zOrder, then age
    bool operator()(const shared_ptr<ParticleInstance> a,
                    const shared_ptr<ParticleInstance> b) const {
        if (a->zOrder() == b->zOrder())
            return a->age < b->age;
        return a->zOrder() < b->zOrder();
    }
};

// This is a *type* of emitter
class EmitterType {
public:
    shared_ptr<ParticleType> type;
    double prob;

    EmitterType(shared_ptr<ParticleType> type, double prob);
};

// This is a particular instance of a physical emitter
class EmitterInstance {
public:
    shared_ptr<EmitterType> emitter;
    double age;

    R2 position;
    R2 velocity;
    double rotation;

    bool freezeVisuals;

    EmitterInstance(shared_ptr<EmitterType> emitter, R2 position,
                    double rotation = 0, R2 velocity = R2(0, 0),
                    bool freezeVisuals = false);

    int tick(double dt);
};

/*
// TODO - consider the design
// Should this class exist, or is the current solution good?
class GravitationalAttractor {
public:
    double mass;
    R2 position;

    // ...
};
*/

// This is the manager.
class ParticleSystem {
    std::vector<shared_ptr<ParticleInstance> > particles;
    std::vector<shared_ptr<EmitterInstance> > emitters;

public:
    ParticleSystem();

    void addParticleInstance(shared_ptr<ParticleInstance> instance);
    void addEmitterInstance(shared_ptr<EmitterInstance> instance);

    void tickEmitter(shared_ptr<EmitterInstance> emitter, double dt);

    void tick(double dt);
    void draw();

    void applyGravity(R2 position, double mass);

    long particleCount();
};
