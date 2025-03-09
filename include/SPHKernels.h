#pragma once

#include <glm/glm.hpp>
#include <cmath>

namespace SPHKernels {
    // Smoothing kernel parameters
    constexpr float PI = 3.14159265358979323846f;
    
    // Poly6 kernel for density calculation
    class Poly6 {
    public:
        static float W(const glm::vec2& r, float h) {
            float r_len = glm::length(r);
            if (r_len >= h) return 0.0f;
            
            float h2 = h * h;
            float h4 = h2 * h2;
            float h9 = h4 * h4 * h;
            float coeff = 4.0f / (PI * h9);
            
            float q = h2 - r_len * r_len;
            return coeff * q * q * q;
        }
        
        static glm::vec2 gradW(const glm::vec2& r, float h) {
            float r_len = glm::length(r);
            if (r_len >= h || r_len < 0.0001f) return glm::vec2(0.0f);
            
            float h2 = h * h;
            float h4 = h2 * h2;
            float h9 = h4 * h4 * h;
            float coeff = -24.0f / (PI * h9);
            
            float q = h2 - r_len * r_len;
            return coeff * q * q * r;
        }
    };
    
    // Spiky kernel for pressure force calculation
    class Spiky {
    public:
        static float W(const glm::vec2& r, float h) {
            float r_len = glm::length(r);
            if (r_len >= h) return 0.0f;
            
            float h5 = h * h * h * h * h;
            float coeff = 10.0f / (PI * h5);
            
            float q = h - r_len;
            return coeff * q * q * q;
        }
        
        static glm::vec2 gradW(const glm::vec2& r, float h) {
            float r_len = glm::length(r);
            if (r_len >= h || r_len < 0.0001f) return glm::vec2(0.0f);
            
            float h5 = h * h * h * h * h;
            float coeff = -30.0f / (PI * h5);
            
            float q = h - r_len;
            return coeff * q * q * (r / r_len);
        }
    };
    
    // Viscosity kernel for viscosity force calculation
    class Viscosity {
    public:
        static float W(const glm::vec2& r, float h) {
            float r_len = glm::length(r);
            if (r_len >= h) return 0.0f;
            
            float h2 = h * h;
            float h3 = h2 * h;
            float coeff = 40.0f / (PI * h2 * h3);
            
            float q = r_len / h;
            return coeff * (1.0f - q) * (1.0f - q) * (1.0f - q);
        }
        
        static float laplacianW(const glm::vec2& r, float h) {
            float r_len = glm::length(r);
            if (r_len >= h) return 0.0f;
            
            float h2 = h * h;
            float h5 = h2 * h2 * h;
            float coeff = 40.0f / (PI * h5);
            
            float q = r_len / h;
            return coeff * (1.0f - q);
        }
    };
} 