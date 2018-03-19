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
target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${target_name} PUBLIC D3D_Tools)
target_link_libraries(${target_name} PUBLIC EverydayTools)
target_link_libraries(${target_name} PUBLIC WinWrappers)
target_link_libraries(${target_name} PUBLIC RapidXML)

if(${CopyAssets})
	add_dependencies(${target_name} CopyAssets)
endif()

if(${CopyConfigs})
	add_dependencies(${target_name} CopyConfigs)
endif()

set_target_properties(Keng PROPERTIES FOLDER Keng)