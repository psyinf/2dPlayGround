# Store warning suppression flags for external dependencies
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(EXTERNAL_PROJECT_COMPILE_OPTIONS
        -Wno-deprecated-declarations     # For std::aligned_storage in BehaviorTree.cpp
        -Wno-dangling-pointer           # For FreeType pointer warnings
        -Wno-misleading-indentation     # For miniz.h in SDL_image
        -Wno-unused-but-set-variable    # For stb_image unused variables
        -Wno-stringop-overflow          # For stb_image string operations
        -Wno-stringop-truncation        # For PhysFS strncpy
        -Wno-conversion                 # For OpenAL bitfield conversions
        -Wno-sign-compare               # For soundEngine size comparisons
        -Wno-unknown-pragmas            # For MSVC pragmas in soundEngine
        -Wno-unused-variable            # For unused variables in soundEngine
    )
endif()

function(get_all_targets var)
    set(targets)
    get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    list(APPEND ${targets} ${current_targets})
endmacro()

function(make_all_targets_system)
    get_all_targets(all_targets)

    foreach(target ${all_targets})
        get_target_property(target_type ${target} TYPE)
        get_target_property(target_source_dir ${target} SOURCE_DIR)
        
        # Check if target is from CPM cache (external dependency)
        string(FIND "${target_source_dir}" "/.cache/cpm/" is_external)
        
        # Skip INTERFACE and UTILITY targets
        if(NOT target_type MATCHES "INTERFACE_LIBRARY|UTILITY")
            # Mark as system includes
            get_property(${target}_include_dirs TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
            if(${target}_include_dirs)
                set_property(TARGET ${target} PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${${target}_include_dirs})
            endif()
            set_target_properties(${target} PROPERTIES SYSTEM TRUE)
            
            # Apply warning suppression to external targets
            if(is_external GREATER -1 AND EXTERNAL_PROJECT_COMPILE_OPTIONS)
                target_compile_options(${target} PRIVATE ${EXTERNAL_PROJECT_COMPILE_OPTIONS})
            endif()
        endif()
    endforeach()
endfunction()