# 2dPlayGround

A small playground to create a mini-game based on SDL, entt using modern C++, cmake, CPM 

## Prerequisites
The project uses [CMake Package Manager](https://github.com/cpm-cmake/CPM.cmake "CMake Package Manager")
As the project uses some advanced C++ features (aiming to use C++23) a recent compiler is needed. See next section for details

## Setting up the build environment Linux/via WSL
The process is tested on Windows using VS2022 and inside an Ubuntu environment (22.04) but might also work on other distributions.
Currently this requires a recent compiler (GCC> 14.1, Clang 17.0.3) or the MSVC 2022 compiler. 

In order to setup your apt linux environment check the /doc/setup.sh script.
It basically get's all the needed packages to get building as well as Conan up and running.

## Build
The project uses CMake and comes with presets, so building is a trivial as:

`cmake . --preset=unixlike-gcc-release`
`cmake --build --preset=unixlike-gcc-release`

Visual Studio and VS Code will also work either directly for Windows or using WSL. 

##Project structure
TODO
##Running 
TODO

## Troubleshooting
In case the initial WSL based configure build of conan hangs (usually when using dpkg like `[CMake] dpkg-query: no packages found matching libegl-dev`) configure once using:
`cmake . --preset=unixlike-gcc-release`


