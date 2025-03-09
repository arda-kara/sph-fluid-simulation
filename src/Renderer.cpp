#include "Renderer.h"
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Vertex shader source
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec3 aColor;
    
    out vec3 particleColor;
    
    uniform mat4 projection;
    
    void main() {
        gl_Position = projection * vec4(aPos, 0.0, 1.0);
        gl_PointSize = 10.0;
        particleColor = aColor;
    }
)";

// Fragment shader source
const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 particleColor;
    out vec4 FragColor;
    
    void main() {
        // Create a circular point
        vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
        if (dot(circCoord, circCoord) > 1.0) {
            discard;
        }
        
        FragColor = vec4(particleColor, 1.0);
    }
)";

Renderer::Renderer(int width, int height, const char* title)
    : width(width), height(height), title(title), window(nullptr),
      shaderProgram(0), vbo(0), colorVbo(0), vao(0) {
}

Renderer::~Renderer() {
    // Clean up OpenGL resources
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &colorVbo);
    glDeleteProgram(shaderProgram);
    
    // Terminate GLFW
    glfwTerminate();
}

bool Renderer::initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }
    
    // Create shader program
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (!shaderProgram) {
        std::cerr << "Failed to create shader program" << std::endl;
        return false;
    }
    
    // Create vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Create vertex buffer object for positions
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Create vertex buffer object for colors
    glGenBuffers(1, &colorVbo);
    glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Unbind VAO
    glBindVertexArray(0);
    
    // Enable point size
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

void Renderer::render(const std::vector<Particle>& particles) {
    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Use shader program
    glUseProgram(shaderProgram);
    
    // Set projection matrix (map simulation coordinates to screen coordinates)
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Prepare position data
    std::vector<glm::vec2> positions;
    positions.reserve(particles.size());
    for (const auto& p : particles) {
        positions.push_back(p.position);
    }
    
    // Prepare color data based on density
    std::vector<glm::vec3> colors;
    colors.reserve(particles.size());
    for (const auto& p : particles) {
        // Map density to color (blue to cyan to white)
        float normalizedDensity = std::min(p.density / 1500.0f, 1.0f);
        glm::vec3 color(normalizedDensity, 0.5f + 0.5f * normalizedDensity, 1.0f);
        colors.push_back(color);
    }
    
    // Update position buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), positions.data(), GL_DYNAMIC_DRAW);
    
    // Update color buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);
    
    // Draw particles
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(particles.size()));
    glBindVertexArray(0);
    
    // Swap buffers
    glfwSwapBuffers(window);
    
    // Poll for events
    glfwPollEvents();
}

bool Renderer::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Renderer::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

GLuint Renderer::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    // Check for compilation errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint Renderer::createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
    // Compile shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    if (!vertexShader) return 0;
    
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return 0;
    }
    
    // Create shader program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Check for linking errors
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }
    
    // Delete shaders (they're linked to the program now)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
} 