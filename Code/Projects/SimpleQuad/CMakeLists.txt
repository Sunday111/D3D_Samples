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

target_link_libraries(${target_name} PRIVATE D3D_Tools)
target_link_libraries(${target_name} PRIVATE EverydayTools)
target_link_libraries(${target_name} PRIVATE ResourceSystem)
target_link_libraries(${target_name} PRIVATE SystemsApp)
target_link_libraries(${target_name} PRIVATE WindowSystem)
target_link_libraries(${target_name} PRIVATE WinWrappers)
target_link_libraries(${target_name} PRIVATE RapidXML)
target_link_libraries(${target_name} PRIVATE Keng)

if(${CopyAssets})
	add_dependencies(${target_name} CopyAssets)
endif()

if(MSVC)
	target_compile_options(${target_name} PUBLIC "/std:c++latest")
	target_compile_definitions(${target_name} PRIVATE "-D_SCL_SECURE_NO_WARNINGS")
	target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
