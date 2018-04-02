cmake_minimum_required(VERSION 3.5.1)

include(generate_vs_filters)

set(module_name Yasli)
set(${module_name} true CACHE BOOL "Wrappers for raw Win32 calls")
if(NOT ${${module_name}})
    return()
endif()

add_library(${module_name} INTERFACE)
target_include_directories(${module_name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${module_name} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/yasli-$<CONFIG>.lib")
