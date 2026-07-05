# String wave FDM

Finite-difference solver and OpenGL/GLUT visualization for the one-dimensional
wave equation with fixed ends. Eigen is included as a Git submodule.

## Build and test

```sh
git submodule update --init --recursive
cmake --preset msys2-clang64
cmake --build --preset msys2-clang64
ctest --preset msys2-clang64
```

Run the simulation and visualization:

```sh
./build/msys2-clang64/string-wave
```

The physical and grid parameters remain constants in `main/main.cpp`, as in the
original project. OpenGL and GLUT (or FreeGLUT) must be installed on the system.
The included Windows preset uses the installed MSYS2 Clang64 toolchain and keeps
it separate from Visual Studio build files.

## Architecture

The project separates numerical integration from presentation:

- `WaveSimulation` owns the finite-difference operator and the three rolling
  displacement vectors. It has no dependency on OpenGL or GLUT.
- `WaveProblem` is supplied by the application and contains all simulation
  constants together with the initial and boundary-condition functions.
- `SimulationObserver` defines the notification contract for a completed time
  step.
- `OpenGLVisualizer` implements `SimulationObserver`. Its GLUT timer advances
  the simulation, while observer notifications request screen redraws.
- `main.cpp` is the composition root that creates the simulation and visualizer.

This direction of dependency keeps the mathematical model reusable in tests or
other frontends. A new observer can record selected frames, calculate energy,
or export data without changing `WaveSimulation`.

## Mathematical model

Let $u(x,t)$ be the transverse displacement of a string of length $L$. The
simulation solves the one-dimensional wave equation

$$
\frac{\partial^2 u}{\partial t^2}
= c^2\frac{\partial^2 u}{\partial x^2},
\qquad 0 \le x \le L.
$$

Here $c$ is the wave speed. For an ideal string,

$$
c = \sqrt{\frac{F}{\rho}},
$$

where $F$ is the tension and $\rho$ is the linear mass density. The program
specifies $c$ directly rather than modelling $F$ and $\rho$ separately.

The boundary conditions are

$$
u(0,t)=g(t), \qquad u(L,t)=g_1(t).
$$

Both functions currently return zero, so the string is fixed at both ends. The
initial conditions are

$$
u(x,0)=f(x), \qquad
\frac{\partial u}{\partial t}(x,0)=f_1(x).
$$

The problem configured in `main.cpp` supplies zero initial velocity and an
initial-displacement function equal to $0.1$ at the centre grid point and zero
elsewhere. This represents a stationary string with a localised initial
displacement. Because the displacement occupies one grid point instead of a
smooth profile, it excites many high-frequency spatial modes.

## Finite-difference method

Space and time are discretised on uniform grids:

$$
x_i=ih, \qquad t_k=k\tau, \qquad u_i^k \approx u(x_i,t_k).
$$

For every interior point, second-order central differences give

$$
\frac{u_i^{k+1}-2u_i^k+u_i^{k-1}}{\tau^2}
=c^2\frac{u_{i+1}^k-2u_i^k+u_{i-1}^k}{h^2}.
$$

Solving for the next time layer produces the explicit update

$$
u_i^{k+1}
=2(1-r^2)u_i^k
+r^2\left(u_{i-1}^k+u_{i+1}^k\right)
-u_i^{k-1},
\qquad
r=\frac{c\tau}{h}.
$$

The implementation writes this update in matrix form:

$$
\mathbf{u}^{k+1}=A\mathbf{u}^k-\mathbf{u}^{k-1}.
$$

Eigen stores $A$ as a sparse tridiagonal matrix. Its interior rows contain
$r^2$, $2(1-r^2)$, and $r^2$. After each step, the two endpoint values are
replaced by $g(t_{k+1})$ and $g_1(t_{k+1})$.

The current implementation initializes the first time layer as

$$
\mathbf{u}^{1}=\mathbf{u}^{0}+\tau\mathbf{f}_1.
$$

Since the current initial velocity is zero, $\mathbf{u}^{1}=\mathbf{u}^{0}$.
The explicit scheme is stable when the Courant condition holds:

$$
r=\frac{c\tau}{h}\le 1.
$$

## Current parameters

| Parameter | Value | Meaning |
|---|---:|---|
| $L$ | `0.5` | string length |
| $T$ | `10` | simulated duration |
| $c$ | `0.2` | wave speed |
| $h$ | `0.0005` | spatial step |
| $\tau$ | `0.00025` | time step |
| $N$ | `1001` | number of spatial points |
| $R$ | `40001` | number of time layers |
| $r$ | `0.1` | Courant number |

## Physical interpretation

The initial displacement separates into two waves travelling from the centre
towards the fixed endpoints. A wave reflected from a fixed endpoint changes
sign and travels back along the string.

This is an ideal model: it contains no damping, external forcing, bending
stiffness, or nonlinear effects. Its physical energy is therefore expected to
remain constant up to discretisation and floating-point errors.

OpenGL renders each computed time layer as the curve $u(x,t)$. The horizontal
axis represents position along the string, and the vertical axis represents
transverse displacement.

The simulation advances one time layer before each rendered frame. Only the
previous, current, and next displacement vectors are retained, so the time
integration requires $3N$ floating-point values instead of storing the full
$N \times R$ history. With the current grid, this reduces displacement storage
from approximately 320 MB to 24 KB.
