cmake_minimum_required(VERSION 3.5.1)

include(generate_vs_filters)

set(module_name FreeType)
set(${module_name} true CACHE BOOL "To deal with fonts")
if(NOT ${${module_name}})
    return()
endif()

add_library(${module_name} INTERFACE)
target_include_directories(${module_name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${module_name} INTERFACE
	debug "${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug/freetyped.lib"
	optimized "${CMAKE_CURRENT_SOURCE_DIR}/lib/Release/freetype.lib")
