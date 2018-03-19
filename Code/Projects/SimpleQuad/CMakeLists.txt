cmake_minimum_required(VERSION 3.5.1)

set(PROJECT_SimpleQuad true CACHE BOOL "Render quad")

if(NOT ${PROJECT_SimpleQuad})
    return()
endif()

set(target_name SimpleQuad)
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_exts "h" "cpp")
set(target_sources)

foreach(entry ${sources_exts})
	file(GLOB_RECURSE sources_by_ext "${sources_dir}/*.${entry}")
	list(APPEND target_sources ${sources_by_ext})
endforeach()

add_library(${target_name} MODULE ${target_sources})

target_link_libraries(${target_name} PRIVATE Keng)

if(${CopyAssets})
	add_dependencies(${target_name} CopyAssets)
endif()

target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
