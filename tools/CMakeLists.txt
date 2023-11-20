include (CheckCXXSymbolExists)
CHECK_CXX_SYMBOL_EXISTS(__cpp_lib_ranges_enumerate ranges has_ranges_enumerate)
CHECK_CXX_SYMBOL_EXISTS(__cpp_lib_ranges_chunk ranges has_ranges_chunk)
#CHECK_CXX_SYMBOL_EXISTS(__cpp_lib_ranges_to_container ranges has_ranges_to)

if (has_ranges_enumerate AND has_ranges_chunk) #AND has_ranges_to)
	add_subdirectory(collision_shape_maker)
endif()		