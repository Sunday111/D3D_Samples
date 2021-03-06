cmake_minimum_required(VERSION 3.5.1)
file(GLOB keng_modules "${CMAKE_CURRENT_SOURCE_DIR}/*")
set(added_subdirectories "")

function(add_keng_module module_name)
    add_subdirectory(${module_name})
    set_target_properties(${added_target_name} PROPERTIES FOLDER Keng)
    list(APPEND added_subdirectories ${added_target_name})
endfunction()

function(add_interface_module module_name)
    add_subdirectory(${module_name})
    list(APPEND added_subdirectories ${added_target_name})
endfunction()

add_interface_module(Base)
add_keng_module(Memory)
add_keng_module(Core)
add_keng_module(Filesystem)
add_keng_module(ResourceSystem)
add_keng_module(GPU)
add_keng_module(Graphics)
add_keng_module(GraphicsCommon)
add_keng_module(WindowSystem)

set(found_projects ${added_subdirectories} PARENT_SCOPE)