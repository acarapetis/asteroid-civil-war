#include "particles.hpp"

ParticleType::ParticleType(shared_ptr<Drawable> visual) : 
    visual(visual),
    lifetime(2), obeyGravity(false),
    drag(0), initialBlender(alphaBlender(WHITE)),
    finalBlender(alphaBlender(WHITE)),
    zOrder(0),
    initialVelocity(R2()), randomVelocity(R2()), acceleration(R2()),
    initialRotation(0), randomRotation(0), deltaRotation(0),
    initialScale(1), randomScale(0), deltaScale(1)
{
}
BlendMode ParticleType::interpolateBlender(double age) {
    return initialBlender.interpolateTo(finalBlender, age/lifetime);
}

void ParticleInstance::draw() {
    this->blender().apply();
    this->visual->draw(this->getCenter(), this->getRotation(), this->getScale());
}

BlendMode ParticleInstance::blender() {
    return this->type->interpolateBlender(this->age);
}
bool ParticleInstance::expired() {
    return this->type->lifetime > 0 && this->age > this->type->lifetime;
}

ParticleInstance::ParticleInstance(shared_ptr<ParticleType> type, R2 center, R2 velocity, double vRot) 
    : type(type), visual(type->visual), Gravitee(center, velocity, 1, type->obeyGravity), vRot(vRot), age(0)

{

    this->rotation       = this->type->initialRotation + this->vRot
                         + this->type->randomRotation * randFactor();
    this->velocity       = this->type->initialVelocity.rotate(this->vRot)
                         + this->type->randomVelocity.randomise();
    this->scale          = this->type->initialScale
                         + this->type->randomScale * randFactor();
}
ParticleInstance::ParticleInstance(shared_ptr<EmitterInstance> emitter) :
    Gravitee(emitter->position, R2(), 1, emitter->emitter->type->obeyGravity),
    type(emitter->emitter->type), visual(type->visual), vRot(emitter->rotation), 
    age(0)
{
    if (emitter->freezeVisuals) freezeVisual();
    this->rotation       = this->type->initialRotation + this->vRot
                         + this->type->randomRotation * randFactor();
    this->velocity       = this->type->initialVelocity.rotate(this->vRot)
                         + this->type->randomVelocity.randomise()
                         + emitter->velocity;
    this->scale          = this->type->initialScale
                         + this->type->randomScale * randFactor();
}

R2     ParticleInstance::getVelocity() { return velocity; }
double ParticleInstance::getRotation() { return rotation; }
double ParticleInstance::getScale()    { return scale; }
R2     ParticleInstance::getCenter()   { return center; }
void ParticleInstance::tick(double dt) {
    this->center   += this->getVelocity() * dt;
    this->velocity += this->type->acceleration.rotate(this->vRot) * dt;
    this->velocity *= pow(this->type->drag,dt);
    this->rotation += this->type->deltaRotation * dt;
    this->scale    *= pow(this->type->deltaScale,dt);
    this->age += dt;
}
double ParticleInstance::zOrder() const {
    return this->type->zOrder;
}
void ParticleInstance::freezeVisual() {
    visual = visual->freeze();
}

EmitterType::EmitterType(shared_ptr<ParticleType> type, double prob) : type(type), prob(prob) {
}

EmitterInstance::EmitterInstance(shared_ptr<EmitterType> emitter, R2 position, double rotation, R2 velocity, bool freezeVisuals) : 
    emitter(emitter), position(position), velocity(velocity), rotation(rotation), freezeVisuals(freezeVisuals)
{
    this->age = 0;
}
int EmitterInstance::tick(double dt) {
    this->age += dt;
    if (emitter->prob * dt <= 1 && randFactor() < emitter->prob * dt)
        return 1;
    else if (emitter->prob * dt > 1)
        return int(emitter->prob * dt) + ((randFactor() < (emitter->prob * dt - int(emitter->prob * dt))) ? 1: 0);
    else
        return 0;
}


ParticleSystem::ParticleSystem() : particles(std::vector<shared_ptr<ParticleInstance> >()) {
}
void ParticleSystem::addParticleInstance(shared_ptr<ParticleInstance> instance) {
    this->particles.push_back(instance); 
}
void ParticleSystem::addEmitterInstance(shared_ptr<EmitterInstance> instance) {
    this->emitters.push_back(instance); 
}
void ParticleSystem::tickEmitter(shared_ptr<EmitterInstance> emitter, double dt) {
    int n = emitter->tick(dt);
    for (int k = 0; k < n; k++)
        this->addParticleInstance(
            shared_ptr<ParticleInstance>(new ParticleInstance(emitter))
        );
}
void ParticleSystem::tick(double dt) {
    for (unsigned int i = 0; i < emitters.size(); i++)
        this->tickEmitter(emitters[i], dt);

    for (unsigned int i = 0; i < particles.size(); i++)
        particles[i]->tick(dt);

    for (unsigned int i = 0; i < particles.size(); i++)
        if (particles[i]->expired()) {
            particles.erase(particles.begin() + i--);
        }
}
void ParticleSystem::draw() {
    // This seems to be reasonably cheap, but remember it's n.log(n
    // It'd be faster to keep each particle type in its own array
    // But you know, ehhhhhhhhhhhhh
    std::sort(particles.begin(), particles.end(), compareZOrder());
    foreach (shared_ptr<ParticleInstance> p, particles) {
        p->draw();
    }
}
void ParticleSystem::applyGravity(R2 position, double mass) {
    foreach (shared_ptr<ParticleInstance> p, particles) {
        p->gravitateTowards(position, mass);
    }
}
long ParticleSystem::particleCount() {
    return this->particles.size();
}

