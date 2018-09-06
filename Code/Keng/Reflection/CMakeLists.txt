cmake_minimum_required(VERSION 3.5.1)
include(generate_vs_filters)
include(glob_cxx_sources)
include(cxx_version)

set(keng_module_name KengReflection)
set(KENG_${keng_module_name} true CACHE BOOL "Reflection")
if(NOT ${KENG_${keng_module_name}})
    return()
endif()

glob_cxx_sources(${CMAKE_CURRENT_SOURCE_DIR} target_sources)
add_library(${keng_module_name} SHARED "${target_sources}")
generate_vs_filters(${target_sources})

target_include_directories(${keng_module_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${keng_module_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${keng_module_name} PUBLIC EverydayTools)
target_link_libraries(${keng_module_name} PUBLIC KengBase)
target_link_libraries(${keng_module_name} PUBLIC KengCore)

if(${KENG_KengMemory})
    target_link_libraries(${keng_module_name} PRIVATE KengMemory)
    target_compile_definitions(${keng_module_name} PRIVATE "-DUSE_KENG_MEMORY")
endif()

set(added_target_name ${keng_module_name} PARENT_SCOPE)
require_cxx_version(${keng_module_name} 17)
disable_cxx_extensions(${keng_module_name})
