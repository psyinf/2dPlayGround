project(PhysFSPP)


add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        PhysFSPP.cpp    
        PhysFSPP_File.cpp
        PhysFSPP_Stream.cpp
        
    PUBLIC
        PhysFSPP.hpp
        PhysFSPP_File.hpp
        PhysFSPP_Stream.hpp
        PhysFSPP_StreamBuf.hpp
        PhysFSPP_Types.hpp
        PhysFSPP_Exceptions.hpp


)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
	    PhysFS::PhysFS-static	
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
