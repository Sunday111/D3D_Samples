cmake_minimum_required(VERSION 3.5.1)
include(generate_vs_filters)

set(keng_module_name KengResourceSystem)
set(KENG_${keng_module_name} true CACHE BOOL "Resource system")
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

add_library(${keng_module_name} SHARED "${target_sources}")
generate_vs_filters(${target_sources})
target_include_directories(${keng_module_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${keng_module_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(${keng_module_name} PRIVATE "-DKENG_RESOURCE_SYSTEM_DLL")
target_link_libraries(${keng_module_name} PUBLIC EverydayTools)
target_link_libraries(${keng_module_name} PUBLIC WinWrappers)
target_link_libraries(${keng_module_name} PUBLIC KengCore)
target_link_libraries(${keng_module_name} PUBLIC KengBase)

set(added_target_name ${keng_module_name} PARENT_SCOPE)
