cmake_minimum_required(VERSION 3.5.1)
include(generate_vs_filters)
include(glob_cxx_sources)

set(keng_module_name KengGraphicsCommon)
set(KENG_${keng_module_name} true CACHE BOOL "GraphicsCommon")
if(NOT ${KENG_${keng_module_name}})
    return()
endif()

glob_cxx_sources(${CMAKE_CURRENT_SOURCE_DIR} target_sources)
add_custom_target(${keng_module_name} SOURCES ${target_sources})
generate_vs_filters(${target_sources})
set(added_target_name ${keng_module_name} PARENT_SCOPE)