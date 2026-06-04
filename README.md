# openEM
A terminal tool that intakes key parameters of an antenna and simulates the RF pattern using Maxwell's equations.

## How to use openEM:
- Define all the necessary simulation parameters in simconfig.json
- Make a build/ folder and use cmake to build the necessary files
- Inside build/, use make to create a binary file
- Run it using the ./openEM command inside build/

## Solver:
A FDTD (Finite-Difference Time-Domain) solver was used.The time derivative was approximated using central difference. The wave equation was taken in a discrete time domain, and used in a time-stepping formula.

## Current State

![alt text](_docs/image.png)

# TODO
- [ ] Implement the solver
- [ ] Include automatic meshing from sim_config.json parameters
- [ ] Allow for importing .stl antenna files
