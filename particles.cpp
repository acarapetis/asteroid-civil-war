#include "particles.hpp"

#include <algorithm>
#include <functional>

#include "mathtools.hpp"

using std::bind;
using namespace std::placeholders;
using std::make_shared;
using std::shared_ptr;

BlendMode ParticleType::interpolateBlender(double age) {
    return initialBlender.interpolateTo(finalBlender, age / lifetime);
}
ALLEGRO_COLOR ParticleType::interpolateTint(double age) {
    return lerp(initialTint, finalTint, age / lifetime);
}

void ParticleInstance::draw(const Camera& camera) const {
    this->blender().apply();
    this->visual->draw(this->center, this->rotation, this->scale, camera,
                       this->tint());
}

BlendMode ParticleInstance::blender() const {
    return this->type->interpolateBlender(this->age);
}
ALLEGRO_COLOR ParticleInstance::tint() const {
    return this->type->interpolateTint(this->age);
}
bool ParticleInstance::expired() {
    return this->type->lifetime > 0 && this->age > this->type->lifetime;
}

ParticleInstance::ParticleInstance(shared_ptr<ParticleType> type, R2 center,
                                   R2 velocity, double vRot)
    : Gravitee(center, velocity, 1, type->obeyGravity),
      type(type),
      visual(type->visual),
      age(0),
      vRot(vRot)

{
    this->rotation = this->type->initialRotation + this->vRot +
                     this->type->randomRotation * randFactor();
    this->velocity = this->type->initialVelocity.rotate(this->vRot) +
                     this->type->randomVelocity.randomise();
    this->scale =
        this->type->initialScale + this->type->randomScale * randFactor();
}
ParticleInstance::ParticleInstance(const EmitterInstance& emitter)
    : Gravitee(emitter.position, R2(), 1, emitter.particleType->obeyGravity),
      type(emitter.particleType),
      visual(type->visual),
      age(0),
      vRot(emitter.rotation) {
    if (emitter.freezeVisuals) freezeVisual();
    this->rotation = this->type->initialRotation + this->vRot +
                     this->type->randomRotation * randFactor();
    this->velocity = this->type->initialVelocity.rotate(this->vRot) +
                     this->type->randomVelocity.randomise() + emitter.velocity;
    this->scale =
        this->type->initialScale + this->type->randomScale * randFactor();
}

void ParticleInstance::tick(double dt) {
    this->center += this->velocity * dt;
    this->velocity += this->type->acceleration.rotate(this->vRot) * dt;
    this->velocity *= pow(this->type->drag, dt);
    this->rotation += this->type->deltaRotation * dt;
    this->scale *= pow(this->type->deltaScale, dt);
    this->age += dt;
}
double ParticleInstance::zOrder() const { return this->type->zOrder; }
void ParticleInstance::freezeVisual() { visual = visual->freeze(); }

EmitterType::EmitterType(shared_ptr<ParticleType> type, double prob)
    : type(type), prob(prob) {}

EmitterInstance::EmitterInstance(shared_ptr<ParticleType> particleType,
                                 double prob, R2 position, double rotation,
                                 R2 velocity, bool freezeVisuals)
    : particleType(particleType),
      prob(prob),
      age(0),
      position(position),
      velocity(velocity),
      rotation(rotation),
      freezeVisuals(freezeVisuals) {}
EmitterInstance::EmitterInstance(const EmitterType& emitterType, R2 position,
                                 double rotation, R2 velocity,
                                 bool freezeVisuals)
    : EmitterInstance(emitterType.type, emitterType.prob, position, rotation,
                      velocity, freezeVisuals) {}

int EmitterInstance::tick(double dt) {
    this->age += dt;
    if (prob * dt <= 1 && randFactor() < prob * dt)
        return 1;
    else if (prob * dt > 1)
        return int(prob * dt) +
               ((randFactor() < (prob * dt - int(prob * dt))) ? 1 : 0);
    else
        return 0;
}

ParticleSystem::ParticleSystem() {}
void ParticleSystem::addParticleInstance(ParticleInstance&& instance) {
    this->particles.emplace_back(std::move(instance));
}
void ParticleSystem::addEmitterInstance(shared_ptr<EmitterInstance> instance) {
    this->emitters.push_back(instance);
}
void ParticleSystem::tickEmitter(EmitterInstance& emitter, double dt) {
    int n = emitter.tick(dt);
    for (int k = 0; k < n; k++)
        this->addParticleInstance(ParticleInstance(emitter));
}
void ParticleSystem::tick(double dt) {
    for (const auto& emitter : emitters) tickEmitter(*emitter, dt);
    for (auto& particle : particles) particle.tick(dt);
    std::erase_if(particles, bind(&ParticleInstance::expired, _1));
}
void ParticleSystem::draw(const Camera& camera) {
    // This seems to be reasonably cheap, but remember it's n.log(n
    // It'd be faster to keep each particle type in its own array
    // But you know, ehhhhhhhhhhhhh
    std::sort(particles.begin(), particles.end(), compareZOrder());
    for (const auto& p : particles) {
        p.draw(camera);
    }
}
void ParticleSystem::applyGravity(R2 position, double mass) {
    for (auto& p : particles) {
        p.gravitateTowards(position, mass);
    }
}
long ParticleSystem::particleCount() { return this->particles.size(); }
