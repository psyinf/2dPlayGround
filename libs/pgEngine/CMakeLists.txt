project(pgEngine)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		SDLApp.cpp
		SDLPrimitives.cpp
		SDLSprite.cpp
		SDLKeyStateMap.cpp
		Factories.cpp
		SDLFont.cpp
		FPSCounter.cpp
	PUBLIC
		SDLApp.h
		SDLVec.h
		SDLAppConfig.h
		SDLPrimitives.h
		SDLSprite.h
		Factories.hpp
		SDLKeyStateMap.h 
		SDLBackgoundSprite.h 
		SDLErrorTrace.h
		SDLBounds.h 
		ComputationalGeometry.hpp 
		SpritePixelData.hpp
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
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)