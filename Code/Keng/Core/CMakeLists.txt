cmake_minimum_required(VERSION 3.5.1)
include(generate_vs_filters)

set(keng_module_name KengCore)
set(KENG_${keng_module_name} true CACHE BOOL "Core")
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
target_compile_definitions(${keng_module_name} PRIVATE "-DKENG_CORE_DLL")
target_link_libraries(${keng_module_name} PUBLIC EverydayTools)
target_link_libraries(${keng_module_name} PRIVATE WinWrappers)

if(${KENG_KengMemory})
	target_link_libraries(${keng_module_name} PRIVATE KengMemory)
	target_compile_definitions(${keng_module_name} PRIVATE "-DUSE_KENG_MEMORY")
endif()

set(added_target_name ${keng_module_name} PARENT_SCOPE)
