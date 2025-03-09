#include <iostream>
#include <chrono>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Simulation.h"
#include "Renderer.h"

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Simulation dimensions (same as window for simplicity)
const float SIM_WIDTH = static_cast<float>(WINDOW_WIDTH);
const float SIM_HEIGHT = static_cast<float>(WINDOW_HEIGHT);

// Target frame rate
const float TARGET_FPS = 60.0f;
const float TARGET_FRAME_TIME = 1.0f / TARGET_FPS;

int main() {
    // Create renderer
    Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT, "SPH Fluid Simulation");
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    // Create simulation
    Simulation simulation(SIM_WIDTH, SIM_HEIGHT);
    
    // Initialize simulation with particles
    int numParticles = 1000;
    simulation.initialize(numParticles);
    
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(renderer.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    // Simulation parameters
    float dt = 0.01f;
    glm::vec2 gravity = simulation.getGravity();
    float viscosity = simulation.getViscosity();
    float gasConstant = simulation.getGasConstant();
    float restDensity = simulation.getRestDensity();
    float smoothingRadius = simulation.getSmoothingRadius();
    float dampingCoefficient = simulation.getDampingCoefficient();
    
    // Performance metrics
    float frameTime = 0.0f;
    float simulationTime = 0.0f;
    float renderTime = 0.0f;
    
    // Main loop
    while (!renderer.shouldClose()) {
        // Process input
        renderer.processInput();
        
        // Start frame timing
        auto frameStart = std::chrono::high_resolution_clock::now();
        
        // Start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // ImGui window for simulation parameters
        ImGui::Begin("Simulation Parameters");
        
        // Particle count
        static int newParticleCount = numParticles;
        if (ImGui::SliderInt("Particle Count", &newParticleCount, 100, 5000)) {
            if (newParticleCount != numParticles) {
                numParticles = newParticleCount;
                simulation.initialize(numParticles);
            }
        }
        
        // Time step
        ImGui::SliderFloat("Time Step", &dt, 0.001f, 0.05f, "%.3f");
        
        // Gravity
        if (ImGui::SliderFloat2("Gravity", &gravity.x, -20.0f, 20.0f)) {
            simulation.setGravity(gravity);
        }
        
        // Viscosity
        if (ImGui::SliderFloat("Viscosity", &viscosity, 0.0f, 1.0f)) {
            simulation.setViscosity(viscosity);
        }
        
        // Gas constant (pressure)
        if (ImGui::SliderFloat("Gas Constant", &gasConstant, 100.0f, 10000.0f)) {
            simulation.setGasConstant(gasConstant);
        }
        
        // Rest density
        if (ImGui::SliderFloat("Rest Density", &restDensity, 500.0f, 2000.0f)) {
            simulation.setRestDensity(restDensity);
        }
        
        // Smoothing radius
        if (ImGui::SliderFloat("Smoothing Radius", &smoothingRadius, 0.01f, 0.5f)) {
            simulation.setSmoothingRadius(smoothingRadius);
        }
        
        // Damping coefficient
        if (ImGui::SliderFloat("Damping", &dampingCoefficient, 0.0f, 1.0f)) {
            simulation.setDampingCoefficient(dampingCoefficient);
        }
        
        // Performance metrics
        ImGui::Separator();
        ImGui::Text("Performance");
        ImGui::Text("Frame Time: %.3f ms (%.1f FPS)", frameTime * 1000.0f, 1.0f / frameTime);
        ImGui::Text("Simulation Time: %.3f ms", simulationTime * 1000.0f);
        ImGui::Text("Render Time: %.3f ms", renderTime * 1000.0f);
        
        ImGui::End();
        
        // Update simulation
        auto simStart = std::chrono::high_resolution_clock::now();
        simulation.update(dt);
        auto simEnd = std::chrono::high_resolution_clock::now();
        simulationTime = std::chrono::duration<float>(simEnd - simStart).count();
        
        // Render particles
        auto renderStart = std::chrono::high_resolution_clock::now();
        renderer.render(simulation.getParticles());
        
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        auto renderEnd = std::chrono::high_resolution_clock::now();
        renderTime = std::chrono::duration<float>(renderEnd - renderStart).count();
        
        // Calculate frame time
        auto frameEnd = std::chrono::high_resolution_clock::now();
        frameTime = std::chrono::duration<float>(frameEnd - frameStart).count();
        
        // Cap frame rate
        if (frameTime < TARGET_FRAME_TIME) {
            std::this_thread::sleep_for(std::chrono::duration<float>(TARGET_FRAME_TIME - frameTime));
            frameTime = TARGET_FRAME_TIME;
        }
    }
    
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    return 0;
} 