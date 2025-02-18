project(pybindSandbox)
include(GNUInstallDirs)
#at least 3.13
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

#add_subdirectory(binder)
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PUBLIC
        pybind_main.cpp
)



target_link_libraries(${PROJECT_NAME}
    PUBLIC
    magic_enum::magic_enum
    pybind11::pybind11
    pybind11::embed 
    pgEngine::pgEngine
)
if (ENABLE_COVERAGE)
    enable_coverage(${PROJECT_NAME})
endif()


install(
    FILES
    $<IF:$<CONFIG:Debug>,${Python3_RUNTIME_LIBRARY_DEBUG},${Python3_RUNTIME_LIBRARY}>  # Select DLL based on build type
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

install(TARGETS ${PROJECT_NAME})
