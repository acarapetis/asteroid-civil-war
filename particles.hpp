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
#include <allegro5/allegro_color.h>

#include <algorithm>
#include <map>
#include <utility>

#include "blending.hpp"
#include "camera.hpp"
#include "graphics.hpp"
#include "object.hpp"
#include "point.hpp"

class EmitterInstance;

// Numerically Calculated Particle - all values have a delta function
// applied each frame, additive or multiplicative based on commonsense.
class ParticleType {
public:
    std::shared_ptr<Drawable> visual;
    double lifetime;
    bool obeyGravity;

    R2 initialVelocity;
    R2 randomVelocity;
    R2 acceleration;  // additive
    double drag;      // multiplicative

    double initialRotation;
    double randomRotation;
    double deltaRotation;  // additive

    double initialScale;
    double randomScale;
    double deltaScale;  // multiplicative

    BlendMode initialBlender;
    BlendMode finalBlender;

    ALLEGRO_COLOR initialTint;
    ALLEGRO_COLOR finalTint;

    double zOrder;

    ParticleType(std::shared_ptr<Drawable> visual);

    BlendMode interpolateBlender(double age);
    ALLEGRO_COLOR interpolateTint(double age);
};

class ParticleInstance : public Gravitee {
    std::shared_ptr<ParticleType> type;
    std::shared_ptr<Drawable> visual;

public:
    double age;
    double vRot;

    std::shared_ptr<ParticleType> getType();

    BlendMode blender();
    ALLEGRO_COLOR tint();

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

    void draw(const Camera& camera);
    void tick(double dt);

    // provided by gravitee: void gravitateTowards(R2 position, double mass);
    ParticleInstance(std::shared_ptr<ParticleType> type, R2 center = R2(),
                     R2 velocity = R2(), double vRot = 0);
    ParticleInstance(std::shared_ptr<EmitterInstance> emitter);
};

struct compareZOrder {
    // Sorting relation for order of particle rendering
    // Precedence: zOrder, then age
    bool operator()(const std::shared_ptr<ParticleInstance> a,
                    const std::shared_ptr<ParticleInstance> b) const {
        if (a->zOrder() == b->zOrder()) return a->age < b->age;
        return a->zOrder() < b->zOrder();
    }
};

// This is a *type* of emitter
class EmitterType {
public:
    std::shared_ptr<ParticleType> type;
    double prob;

    EmitterType(std::shared_ptr<ParticleType> type, double prob);
};

// This is a particular instance of a physical emitter
class EmitterInstance {
public:
    std::shared_ptr<EmitterType> emitter;
    double age;

    R2 position;
    R2 velocity;
    double rotation;

    bool freezeVisuals;

    EmitterInstance(std::shared_ptr<EmitterType> emitter, R2 position,
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
    std::vector<std::shared_ptr<ParticleInstance>> particles;
    std::vector<std::shared_ptr<EmitterInstance>> emitters;

public:
    ParticleSystem();

    void addParticleInstance(std::shared_ptr<ParticleInstance> instance);
    void addEmitterInstance(std::shared_ptr<EmitterInstance> instance);

    void tickEmitter(std::shared_ptr<EmitterInstance> emitter, double dt);

    void tick(double dt);
    void draw(const Camera& camera);

    void applyGravity(R2 position, double mass);

    long particleCount();
};
