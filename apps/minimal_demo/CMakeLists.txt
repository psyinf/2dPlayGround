project(minimal_demo)
find_package(SDL2 REQUIRED)
add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME}
    PUBLIC
       main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    sdlpp::sdlpp
    SDL2::SDL2
    SDL2::SDL2main
)

enable_coverage(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        #RUNTIME DESTINATION bin
        #LIBRARY DESTINATION lib
        #ARCHIVE DESTINATION lib/static
)