cmake_minimum_required(VERSION 3.10)
project(SPH_Fluid_Simulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

# Add ImGui source files
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Add source files
set(SOURCES
    src/main.cpp
    src/Particle.cpp
    src/Simulation.cpp
    src/Renderer.cpp
    ${IMGUI_SOURCES}
)

# Add header files
set(HEADERS
    include/Particle.h
    include/Simulation.h
    include/Renderer.h
    include/SPHKernels.h
)

# Create executable
add_executable(sph_simulation ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(sph_simulation ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw)

# Include directories
target_include_directories(sph_simulation PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
) 