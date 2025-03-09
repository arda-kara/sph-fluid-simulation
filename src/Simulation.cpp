#include "Simulation.h"
#include "SPHKernels.h"
#include <random>
#include <algorithm>

Simulation::Simulation(float width, float height)
    : width(width), height(height) {
    // Default simulation parameters
    gravity = glm::vec2(0.0f, -9.81f);
    viscosity = 0.1f;
    gasConstant = 2000.0f;
    restDensity = 1000.0f;
    smoothingRadius = 0.1f;
    dampingCoefficient = 0.5f;
}

Simulation::~Simulation() {
    // Clean up resources if needed
}

void Simulation::initialize(int numParticles) {
    particles.clear();
    particles.reserve(numParticles);
    
    // Random number generator for initial positions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(width * 0.25f, width * 0.75f);
    std::uniform_real_distribution<float> disY(height * 0.5f, height * 0.9f);
    
    // Create particles with random positions in the upper half of the container
    for (int i = 0; i < numParticles; ++i) {
        glm::vec2 position(disX(gen), disY(gen));
        glm::vec2 velocity(0.0f, 0.0f);
        float mass = 1.0f;
        
        particles.emplace_back(position, velocity, mass);
    }
}

void Simulation::update(float dt) {
    // Compute density and pressure
    computeDensityPressure();
    
    // Compute forces
    computeForces();
    
    // Integrate
    integrate(dt);
    
    // Handle boundaries
    handleBoundaries();
}

void Simulation::computeDensityPressure() {
    // For each particle
    for (auto& pi : particles) {
        // Reset density
        pi.density = 0.0f;
        
        // Compute density using Poly6 kernel
        for (const auto& pj : particles) {
            glm::vec2 r = pi.position - pj.position;
            pi.density += pj.mass * SPHKernels::Poly6::W(r, smoothingRadius);
        }
        
        // Compute pressure using equation of state
        pi.pressure = gasConstant * (pi.density - restDensity);
        if (pi.pressure < 0.0f) pi.pressure = 0.0f; // Prevent negative pressure
    }
}

void Simulation::computeForces() {
    // For each particle
    for (auto& pi : particles) {
        // Reset forces
        pi.resetForce();
        
        // Add gravity
        pi.force += gravity * pi.mass;
        
        // For each other particle
        for (const auto& pj : particles) {
            if (&pi == &pj) continue; // Skip self
            
            glm::vec2 r = pi.position - pj.position;
            float r_len = glm::length(r);
            
            // Skip if particles are too far apart
            if (r_len >= smoothingRadius) continue;
            
            // Pressure force using Spiky kernel
            glm::vec2 pressureForce = -pj.mass * (pi.pressure + pj.pressure) / (2.0f * pj.density) * 
                                      SPHKernels::Spiky::gradW(r, smoothingRadius);
            
            // Viscosity force using Viscosity kernel
            glm::vec2 viscosityForce = viscosity * pj.mass * (pj.velocity - pi.velocity) / pj.density * 
                                      SPHKernels::Viscosity::laplacianW(r, smoothingRadius);
            
            // Add forces
            pi.force += pressureForce + viscosityForce;
        }
    }
}

void Simulation::integrate(float dt) {
    // For each particle
    for (auto& p : particles) {
        // Compute acceleration
        glm::vec2 acceleration = p.force / p.density;
        
        // Update velocity (semi-implicit Euler)
        p.velocity += acceleration * dt;
        
        // Update position
        p.position += p.velocity * dt;
    }
}

void Simulation::handleBoundaries() {
    // For each particle
    for (auto& p : particles) {
        // Left boundary
        if (p.position.x < 0.0f) {
            p.position.x = 0.0f;
            p.velocity.x = -p.velocity.x * dampingCoefficient;
        }
        
        // Right boundary
        if (p.position.x > width) {
            p.position.x = width;
            p.velocity.x = -p.velocity.x * dampingCoefficient;
        }
        
        // Bottom boundary
        if (p.position.y < 0.0f) {
            p.position.y = 0.0f;
            p.velocity.y = -p.velocity.y * dampingCoefficient;
        }
        
        // Top boundary
        if (p.position.y > height) {
            p.position.y = height;
            p.velocity.y = -p.velocity.y * dampingCoefficient;
        }
    }
} 