# 2dPlayGround

A small playground to create a mini-game based on SDL, entt using modern C++, cmake, CPM and conan

## Prerequisites
The project uses [CMake Package Manager](https://github.com/cpm-cmake/CPM.cmake "CMake Package Manager") and [Conan](https://conan.io "Conan") for dependencies.

##Setting up the build environment Linux/via WSL
The process is tested in an Ubuntu environment (22.04) but might also work on other distributions.

In order to setup your apt linux environment check the /doc/setup.sh script.
It basically get's all the needed packages to get building as well as Conan up and running.

##Build
The project uses CMake and comes with presets, so building is a trivial as:

`cmake . --preset=unixlike-gcc-debug`

`cmake --build --preset=unixlike-gcc-debug`

Visual Studio and VS Code will also work either directly for Windows or using WSL. 

