cmake_minimum_required(VERSION 3.5.1)

function(run_py_script script_name)
    execute_process(COMMAND ${script_name} ${ARGN})
endfunction()