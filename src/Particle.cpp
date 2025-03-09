#include "Particle.h"

Particle::Particle(const glm::vec2& pos, const glm::vec2& vel, float m)
    : position(pos), velocity(vel), mass(m), density(0.0f), pressure(0.0f) {
    resetForce();
}

void Particle::resetForce() {
    force = glm::vec2(0.0f, 0.0f);
} 