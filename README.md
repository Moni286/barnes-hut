# Barnes-Hut

An object-oriented Barnes-Hut gravity simulator.

## Current Implementation:
- Simulation can be initialized from a JSON file containing required parameters.
- Particle positions and masses are written to stdout (as csv) in a separate logging thread. Data can be saved to a file or piped to a particle animator.

Note: All parameters have default values specified in main.cpp and/or in the Universe::Builder constructor

## Current Benchmark

- Intel Core i7-7700HQ

- 16 GB RAM

|Particle Count|Initialization|Iteration|
| :------------- | :----------: | -----------: |
|  1000 | ~2 ms   |    1.23 ms |
|2000|~3 ms|2.63 ms|
|4000|~6 ms|5.91 ms|
|8000|~12 ms|16.2 ms|
|16000|~25 ms|37.4 ms|




## Plans:
- Handle singularities.
- Implement 3D cells and geometry (should be easy).
- Convert to CUDA project and use GPU for integration, force and CoM computations.
- Implement a memory pool for faster quad/octree initialization.
- Make an Integrator interface and implement basic integrators. (Leapfrog, RK4...)
- Add more initialization parameters to enable starting with a stable galaxy/cluster.
- Save current state and start from previous state.
