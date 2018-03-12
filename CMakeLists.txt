cmake_minimum_required(VERSION 3.5.1)
project(D3D_Samples)
set(root_dir "${CMAKE_CURRENT_SOURCE_DIR}")
set(submodules_dir "${root_dir}/submodules")
set(code_dir "${root_dir}/Code")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)

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

file(GLOB_RECURSE asset_files "${CMAKE_SOURCE_DIR}/Assets/*")

add_custom_target(
	CopyAssets
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Assets "$<TARGET_PROPERTY:Test_Sample,$<CONFIG>_OUTPUT_NAME>/Assets"
	DEPENDS ${asset_files}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMENT "Copy assets to output directory"
	#SOURCES ${asset_files}
)

add_dependencies(Test_Sample CopyAssets)

if(MSVC)
    target_compile_options(Test_Sample PUBLIC "/std:c++latest")
endif()
