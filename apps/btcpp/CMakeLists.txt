project(btcpp)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME}
    PUBLIC
       main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    pgEngine::pgEngine
    pgGame::pgGame
     BT::behaviortree_cpp
)

enable_coverage(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        #RUNTIME DESTINATION bin
        #LIBRARY DESTINATION lib
        #ARCHIVE DESTINATION lib/static
)