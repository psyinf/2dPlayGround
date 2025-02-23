project(resourceLoaderSandBox)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PUBLIC
       main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    pgEngine::pgEngine
    pgGame::pgGame
    PhysFSPP::PhysFSPP
    #vfspp
)

if (ENABLE_COVERAGE)
    enable_coverage(${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME}
        #RUNTIME DESTINATION bin
        #LIBRARY DESTINATION lib
        #ARCHIVE DESTINATION lib/static
)