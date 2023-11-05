cmake_minimum_required(VERSION 3.23)
project(asteroids)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} main.cpp Player.cpp Game.cpp Background.cpp "Asteroids.cpp" )

find_package(fmt REQUIRED)
find_package(EnTT REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    pgEngine::pgEngine
    fmt::fmt
    EnTT::EnTT
)

enable_coverage(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME})