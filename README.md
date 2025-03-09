# SPH Fluid Simulation

A basic 2D Smoothed-Particle Hydrodynamics (SPH) fluid simulation implemented in C++ with OpenGL rendering.

## Features

- 2D fluid simulation using SPH method
- Real-time rendering with OpenGL
- Interactive UI with ImGui for parameter tuning
- Adjustable simulation parameters:
  - Gravity
  - Viscosity
  - Pressure
  - Particle count
  - Smoothing radius
  - Damping coefficient

## Requirements

- C++17 compatible compiler
- OpenGL 3.3+
- GLEW
- GLFW3
- GLM
- ImGui (may be included as a submodule)

## Building

### Clone the repository with submodules

```bash
git clone --recursive https://github.com/arda-kara/sph-fluid-simulation.git
cd sph-fluid-simulation
```

If you've already cloned the repository without the `--recursive` flag, you can fetch the submodules with:

```bash
git submodule update --init --recursive
```

### Build with CMake

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./sph_simulation
```

## Controls

- **ESC**: Exit the application
- **ImGui Interface**: Adjust simulation parameters in real-time

## Implementation Details

### SPH Method

The simulation uses the Smoothed-Particle Hydrodynamics method with the following kernels:
- **Poly6 kernel** for density estimation
- **Spiky kernel** for pressure forces
- **Viscosity kernel** for viscosity forces

### Time Integration

The simulation uses a simple Euler integration method:
```
v_new = v_old + dt * acceleration
x_new = x_old + dt * v_new
```

### Boundary Handling

Particles are reflected when hitting boundaries with a damping coefficient to reduce velocity.

## Performance

The simulation is optimized for CPU performance and should run at 30+ FPS with 1000+ particles on modern hardware.

## Future Improvements

- Spatial hashing for neighbor search optimization
- CUDA acceleration
- 3D simulation
- Surface rendering
- Multiple fluid types
- Rigid body interactions

## License

This project is licensed under the MIT License - see the LICENSE file for details. 