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

set(test_sample_sources_dir ${code_dir}/TestSample/)
set(test_sample_sources_exts "h" "cpp")
set(test_sample_sources)
foreach(entry ${test_sample_sources_exts})
	file(GLOB_RECURSE test_sample_sources_ext "${test_sample_sources_dir}/*.${entry}")
	list(APPEND test_sample_sources ${test_sample_sources_ext})
endforeach()
add_executable(TestSample WIN32 ${test_sample_sources})
add_subdirectory(${submodules_dir})

target_link_libraries(TestSample D3D_Tools)
target_link_libraries(TestSample EverydayTools)
target_link_libraries(TestSample ResourceSystem)
target_link_libraries(TestSample SystemsApp)
target_link_libraries(TestSample WindowSystem)
target_link_libraries(TestSample WinWrappers)

file(GLOB_RECURSE asset_files "${CMAKE_SOURCE_DIR}/Assets/*")

add_custom_target(
	CopyAssets
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Assets "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/Assets"
	DEPENDS ${asset_files}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMENT "Copying assets to output directory"
)

add_dependencies(TestSample CopyAssets)

if(MSVC)
    target_compile_options(TestSample PUBLIC "/std:c++latest")
endif()

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestSample)
set(debug_directory "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

foreach(config ${CMAKE_CONFIGURATION_TYPES})
	set_target_properties(TestSample PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
endforeach()
