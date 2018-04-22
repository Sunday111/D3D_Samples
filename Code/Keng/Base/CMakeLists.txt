cmake_minimum_required(VERSION 3.5.1)
include(generate_vs_filters)

set(keng_module_name KengBase)
set(KENG_${keng_module_name} true CACHE BOOL "Header only library with basic tools")
if(NOT ${KENG_${keng_module_name}})
    return()
endif()

set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_exts "h" "cpp")
set(target_sources)

foreach(entry ${sources_exts})
    file(GLOB_RECURSE sources_by_ext "${sources_dir}/*.${entry}")
    list(APPEND target_sources ${sources_by_ext})
endforeach()

add_library(${keng_module_name} INTERFACE)
generate_vs_filters(${target_sources})
target_include_directories(${keng_module_name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${keng_module_name} INTERFACE EverydayTools)
target_link_libraries(${keng_module_name} INTERFACE Yasli)

set(added_target_name ${keng_module_name} PARENT_SCOPE)
