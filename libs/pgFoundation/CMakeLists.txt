project(pgFoundation)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
#set language to cXX
#Interface only library



set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_sources(${PROJECT_NAME}
    PRIVATE
    PUBLIC
        imgui/Spinner.hpp


)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
		
	PRIVATE
)

target_include_directories(
  ${PROJECT_NAME}
#  PRIVATE
#	${CMAKE_CURRENT_SOURCE_DIR}
  INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
