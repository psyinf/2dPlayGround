project(pgOrbit CXX)

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}		
	PUBLIC
		EulerCoordinates.hpp
		FloatingPointType.hpp
		OrbitalMechanics.hpp
		OrbitalParameters.hpp
		StarParameters.hpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    PRIVATE
)

target_include_directories(
  ${PROJECT_NAME}
		
  PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties( ${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)