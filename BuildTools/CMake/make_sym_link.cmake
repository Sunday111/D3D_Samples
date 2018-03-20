cmake_minimum_required(VERSION 3.5.1)
include(run_py_script)

function(make_sym_link source destination)
    run_py_script("${root_dir}/BuildTools/Python/make_sym_link.py" ${source} ${destination})
endfunction()