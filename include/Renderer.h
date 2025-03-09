#pragma once

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Particle.h"

class Renderer {
public:
    Renderer(int width, int height, const char* title);
    ~Renderer();
    
    // Initialize OpenGL resources
    bool initialize();
    
    // Render particles
    void render(const std::vector<Particle>& particles);
    
    // Check if window should close
    bool shouldClose() const;
    
    // Process input
    void processInput();
    
    // Get window
    GLFWwindow* getWindow() const { return window; }
    
private:
    // Window dimensions
    int width;
    int height;
    const char* title;
    
    // GLFW window
    GLFWwindow* window;
    
    // Shader program
    GLuint shaderProgram;
    
    // Vertex buffer objects
    GLuint vbo;
    GLuint colorVbo;
    
    // Vertex array object
    GLuint vao;
    
    // Compile shader
    GLuint compileShader(GLenum type, const char* source);
    
    // Create shader program
    GLuint createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
}; 