project(pgGame)

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

find_package(fmt REQUIRED)
find_package(EnTT REQUIRED)

target_sources(${PROJECT_NAME}
	PRIVATE
		core/Game.cpp
		core/KeyStateMap.cpp
		core/Scene.cpp
	PUBLIC
		core/Game.hpp
		core/FrameStamp.hpp
		core/RegistryHelper.hpp
		core/KeyStateMap.hpp
		core/ResourceCache.hpp
		core/Scene.hpp
		
		systems/SystemInterface.hpp
		
		components/WindowDetails.hpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
		EnTT::EnTT
	PRIVATE
	    pgEngine::pgEngine
		fmt::fmt
)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
