cmake_minimum_required(VERSION 3.5.1)

set(PROJECT_JustTriangle true CACHE BOOL "Work with textures")

if(NOT ${PROJECT_JustTriangle})
    return()
endif()

set(target_name JustTriangle)
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_exts "h" "cpp")
set(target_sources)

foreach(entry ${sources_exts})
	file(GLOB_RECURSE sources_by_ext "${sources_dir}/*.${entry}")
	list(APPEND target_sources ${sources_by_ext})
endforeach()

add_executable(${target_name} WIN32 ${target_sources})

target_link_libraries(${target_name} PRIVATE D3D_Tools)
target_link_libraries(${target_name} PRIVATE EverydayTools)
target_link_libraries(${target_name} PRIVATE ResourceSystem)
target_link_libraries(${target_name} PRIVATE SystemsApp)
target_link_libraries(${target_name} PRIVATE WindowSystem)
target_link_libraries(${target_name} PRIVATE WinWrappers)

if(${CopyAssets})
	add_dependencies(${target_name} CopyAssets)
endif()

if(MSVC)
	target_compile_options(${target_name} PUBLIC "/std:c++latest")
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${target_name})
	set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
endif()
