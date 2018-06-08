cmake_minimum_required(VERSION 3.5.1)
include(cxx_version)
include(glob_cxx_sources)

set(PROJECT_Launcher true CACHE BOOL "Launcher")

if(NOT ${PROJECT_Launcher})
    return()
endif()

glob_cxx_sources(${CMAKE_CURRENT_SOURCE_DIR} target_sources)
set(target_name Launcher)
add_executable(${target_name} WIN32 ${target_sources})
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target_name} PRIVATE EverydayTools)
target_link_libraries(${target_name} PRIVATE WinWrappers)
target_link_libraries(${target_name} PRIVATE KengCore)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${target_name})
    set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
endif()

if(${KENG_KengMemory})
    target_link_libraries(${target_name} PRIVATE KengMemory)
    target_compile_definitions(${target_name} PRIVATE "-DUSE_KENG_MEMORY")
endif()

set(make_sym_link_script "${root_dir}/BuildTools/Python/make_sym_link.py")

macro(symlink_to_dir dir)
    add_custom_command(
        TARGET  Launcher
        POST_BUILD
        COMMAND "py" "${make_sym_link_script}" "${root_dir}/${dir}" "${bin_dir}/${dir}"
        COMMENT "Make symlink to '${dir}' folder")
endmacro()

symlink_to_dir(Assets)
symlink_to_dir(Configs)

foreach(project ${found_projects})
    add_dependencies(Launcher ${project})
endforeach()

require_cxx_version(${target_name} 17)
disable_cxx_extensions(${target_name})