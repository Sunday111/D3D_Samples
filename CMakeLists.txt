cmake_minimum_required(VERSION 3.5.1)
project(D3D_Samples)
set(root_dir "${CMAKE_CURRENT_SOURCE_DIR}")
set(submodules_dir "${root_dir}/submodules")
set(code_dir "${root_dir}/Code")
add_executable(Test_Sample WIN32
    ${code_dir}/DirectX/Application.h
    ${code_dir}/DirectX/GraphicsSystem.cpp
    ${code_dir}/DirectX/GraphicsSystem.h
    ${code_dir}/DirectX/main.cpp)
add_subdirectory(${submodules_dir})

target_link_libraries(Test_Sample D3D_Tools)
target_link_libraries(Test_Sample EverydayTools)
target_link_libraries(Test_Sample ResourceSystem)
target_link_libraries(Test_Sample SystemsApp)
target_link_libraries(Test_Sample WindowSystem)
target_link_libraries(Test_Sample WinWrappers)

if(MSVC)
    target_compile_options(Test_Sample PUBLIC "/std:c++latest")
endif()