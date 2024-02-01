project(pgEngine)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		SDLApp.cpp
		primitives/Primitives.cpp
		sprite/Sprite.cpp
		SDLKeyStateMap.cpp
		factories/Factories.cpp
		font/Font.cpp
		FPSCounter.cpp
	PUBLIC
		factories/Factories.hpp
		
		font/Font.hpp
		
		math/Bounds.hpp
		math/BoundsOperations.hpp
		math/ComputationalGeometry.hpp 
		math/Random.hpp
		math/Vec.hpp
		math/VecOperations.hpp
		primitives/Primitives.hpp

		sprite/BackgoundSprite.hpp
		sprite/Sprite.hpp
		sprite/SpritePixelData.hpp

		SDLApp.h
		SDLAppConfig.h
		
		
		SDLKeyStateMap.h 
		
		SDLErrorTrace.h


)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    sdlpp::sdlpp
    SDL2::SDL2
    SDL2::SDL2main
	SDL2_image::SDL2_image-static
	PRIVATE
	SDL2_ttf::SDL2_ttf-static
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)