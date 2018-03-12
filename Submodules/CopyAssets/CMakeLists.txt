cmake_minimum_required(VERSION 3.5.1)
set(module_name CopyAssets)
set(CopyAssets true CACHE BOOL "Copy assets to output directory")
if(NOT ${CopyAssets})
    return()
endif()

file(GLOB_RECURSE asset_files "${assets_dir}/*")

add_custom_target(
	CopyAssets
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${assets_dir}" "${bin_dir}/$<CONFIG>/Assets"
	DEPENDS ${asset_files}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMENT "Copying assets to output directory"
)