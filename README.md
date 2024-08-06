# 2dPlayGround

A small playground 2D game engine to create a mini-game that is based on SDL2, ImGu, [EnTT](https://github.com/skypjack/entt) and an minimal custom made OpenAL sound-engine.\
In principal I tried to stick modern C++ wherever it made sense, mainly to keep myself up2date with new possibilities to make the code more readable, efficient and safer.\

## Prerequisites
The project uses [CMake Package Manager](https://github.com/cpm-cmake/CPM.cmake "CMake Package Manager")\
As the project uses some advanced C++ features (aiming to use C++23) a recent compiler is needed. See next section for details

* CMake > 3.22
* GCC > 14.1 or Clang 17.0.3 or the MSVC 2022 compiler

It is tested on Windows using VS2022 and inside an Ubuntu environment (24.04) but might also work on other distributions. 

## Build
The project uses CMake and comes with presets, so building is a trivial as:

```
cmake --preset=unixlike-gcc-release
cmake --build --preset=unixlike-gcc-release
```
All available presets for your target platform:
```
cmake --list-presets
```

Visual Studio and VS Code will also work either directly for Windows or using WSL. 

## Project structure

The project currently consists of 3 major parts/folders
* libs
  - pgFoundation
  - pgEngine
  - pgGame
* apps
  - sandbox
  - galaxy
  - asteroids
  - ... some other
* tools
* tests
* 
### tests
Catch2 based unittest. By far not complete. 

### tools
All tools for creating resources etc. 
Currently there is only a collisionShape generator that produces convex hulls from sprites.

### libs
#### pgFoundation
Comprises of some classes and facilties that are not directly specific to this. Is a candidate to be moved outside the project
```bash
├── caching      
├── imgui
├── paths
├── serialization
└── strings
```
#### pgEngine
Some attempt to build a minimal game engine which in principal is based on SDL2 (in detail it is using [sdlpp](https://github.com/mika314/sdlpp)).
It provides core an core architecture by abstracting an application, event loop, math and rendering primitives.

The math library contains a minimal linear algebra library (as i found [GMTL](https://ggt.sourceforge.net)/[my GMTL fork](https://github.com/psyinf/gmtl) clumsy and outdated and [eigen](https://github.com/PX4/eigen) to much for what I wanted initially. I've also considered [GLM](https://github.com/g-truc/glm) ). With it I added some minimal space partitioning (a KDTree) and some minimal collision code. 

#### pgGame
Extends the pgEngine using ECS (EnTT) to provide basic entities, a system abstraction and basic resource and scene management. 

### apps
#### Asteroids
This serves as a simple tech demo and concept playground. It is based on pgGame and therefore is a based on an EnTT. There is still a lot to learn and to experiment with.

#### Galaxy
A small portion of a game idea I developed some years ago. It basically revolves around [vonNeumann probes](https://en.wikipedia.org/wiki/Self-replicating_spacecraft) and is currently developed into a working game. 

### Sandbox et al. 
All other folders are for testing, messing around and minimal tech demos. 

## Running 
If you want to try, you can use the install targets created by the cmake-configure step. They will install all resources, libraries. 

## Troubleshooting



