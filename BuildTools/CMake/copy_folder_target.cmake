cmake_minimum_required(VERSION 3.5.1)

function(copy_folder_target target_name from_dir to_dir description)
    file(GLOB_RECURSE files "${from_dir}/*")
    add_custom_target(
        ${target_name}
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${from_dir}" "${to_dir}"
        DEPENDS ${files}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT ${description})
endfunction()
