cmake_minimum_required(VERSION 3.5.1)
include(generate_vs_filters)
include(glob_cxx_sources)

set(keng_module_name KengGraphicsCommon)
set(KENG_${keng_module_name} true CACHE BOOL "GraphicsCommon")
if(NOT ${KENG_${keng_module_name}})
    return()
endif()

add_library(${keng_module_name} INTERFACE)

target_include_directories(${keng_module_name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

glob_cxx_sources(${CMAKE_CURRENT_SOURCE_DIR} target_sources)
set(custom_target_name ${keng_module_name}_)
add_custom_target(${custom_target_name} SOURCES ${target_sources})
generate_vs_filters(${target_sources})
set(added_target_name ${keng_module_name}_ PARENT_SCOPE)