#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Particle.h"

class Simulation {
public:
    Simulation(float width, float height);
    ~Simulation();
    
    // Initialize the simulation with a given number of particles
    void initialize(int numParticles);
    
    // Update the simulation by one time step
    void update(float dt);
    
    // Get the particles for rendering
    const std::vector<Particle>& getParticles() const { return particles; }
    
    // Simulation parameters
    void setGravity(const glm::vec2& g) { gravity = g; }
    void setViscosity(float v) { viscosity = v; }
    void setGasConstant(float k) { gasConstant = k; }
    void setRestDensity(float rho0) { restDensity = rho0; }
    void setSmoothingRadius(float h) { smoothingRadius = h; }
    void setDampingCoefficient(float d) { dampingCoefficient = d; }
    
    // Getters for simulation parameters
    glm::vec2 getGravity() const { return gravity; }
    float getViscosity() const { return viscosity; }
    float getGasConstant() const { return gasConstant; }
    float getRestDensity() const { return restDensity; }
    float getSmoothingRadius() const { return smoothingRadius; }
    float getDampingCoefficient() const { return dampingCoefficient; }
    
private:
    // Compute density and pressure for all particles
    void computeDensityPressure();
    
    // Compute forces for all particles
    void computeForces();
    
    // Integrate particles forward in time
    void integrate(float dt);
    
    // Handle boundary conditions
    void handleBoundaries();
    
    // Container dimensions
    float width;
    float height;
    
    // Particles
    std::vector<Particle> particles;
    
    // Simulation parameters
    glm::vec2 gravity;              // Gravity force
    float viscosity;                // Viscosity coefficient
    float gasConstant;              // Gas constant for pressure calculation
    float restDensity;              // Rest density
    float smoothingRadius;          // Smoothing radius for kernels
    float dampingCoefficient;       // Damping coefficient for boundary collisions
}; 