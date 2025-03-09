#pragma once

#include <glm/glm.hpp>

struct Particle {
    glm::vec2 position;     // Position in 2D space
    glm::vec2 velocity;     // Velocity vector
    glm::vec2 force;        // Accumulated force
    float mass;             // Particle mass
    float density;          // Fluid density at particle position
    float pressure;         // Pressure at particle position
    
    // Constructor with default values
    Particle(const glm::vec2& pos = glm::vec2(0.0f), 
             const glm::vec2& vel = glm::vec2(0.0f),
             float m = 1.0f);
    
    // Reset forces to zero
    void resetForce();
}; 