project(productionSandbox)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME}
    PUBLIC
       main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    pgEngine::pgEngine
    pgGame::pgGame
    pgf::pgf
    magic_enum::magic_enum
)

if (ENABLE_COVERAGE)
    enable_coverage(${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME})