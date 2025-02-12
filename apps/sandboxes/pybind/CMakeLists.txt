project(pybindSandbox)


#add_subdirectory(binder)
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PUBLIC
        pybind_main.cpp
)

pybind11_add_module(libmath MODULE binder/bindings.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    magic_enum::magic_enum
    pybind11::pybind11
    pybind11::embed
)
if (ENABLE_COVERAGE)
    enable_coverage(${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME})