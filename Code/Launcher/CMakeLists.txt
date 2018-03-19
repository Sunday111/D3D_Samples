cmake_minimum_required(VERSION 3.5.1)

set(PROJECT_Launcher true CACHE BOOL "Launcher")

if(NOT ${PROJECT_Launcher})
    return()
endif()

set(target_name Launcher)
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_exts "h" "cpp")
set(target_sources)

foreach(entry ${sources_exts})
	file(GLOB_RECURSE sources_by_ext "${sources_dir}/*.${entry}")
	list(APPEND target_sources ${sources_by_ext})
endforeach()

add_executable(${target_name} WIN32 ${target_sources})
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target_name} PRIVATE EverydayTools)
target_link_libraries(${target_name} PRIVATE WinWrappers)

if(MSVC)
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${target_name})
	set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
endif()
