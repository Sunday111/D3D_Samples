cmake_minimum_required(VERSION 3.5.1)

set(PROJECT_Keng true CACHE BOOL "Work with textures")

if(NOT ${PROJECT_Keng})
    return()
endif()

set(target_name Keng)
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_exts "h" "cpp")
set(target_sources)

foreach(entry ${sources_exts})
	file(GLOB_RECURSE sources_by_ext "${sources_dir}/*.${entry}")
	list(APPEND target_sources ${sources_by_ext})
endforeach()

add_library(${target_name} STATIC ${target_sources})

target_link_libraries(${target_name} PUBLIC D3D_Tools)
target_link_libraries(${target_name} PUBLIC EverydayTools)
target_link_libraries(${target_name} PUBLIC ResourceSystem)
target_link_libraries(${target_name} PUBLIC SystemsApp)
target_link_libraries(${target_name} PUBLIC WindowSystem)
target_link_libraries(${target_name} PUBLIC WinWrappers)
target_link_libraries(${target_name} PUBLIC RapidXML)

if(${CopyAssets})
	add_dependencies(${target_name} CopyAssets)
endif()

if(MSVC)
	target_compile_options(${target_name} PUBLIC "/std:c++latest")
	target_compile_definitions(${target_name} PUBLIC "-D_SCL_SECURE_NO_WARNINGS")
	target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${target_name})
	set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
endif()
