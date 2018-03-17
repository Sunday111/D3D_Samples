cmake_minimum_required(VERSION 3.5.1)
file(GLOB projects "${CMAKE_CURRENT_SOURCE_DIR}/*")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Keng)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Launcher)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Projects)

#Why???
#add_dependencies(Launcher SimpleQuad)
#add_dependencies(Launcher TextureQuad)

