cmake_minimum_required(VERSION 3.5.1)

add_subdirectory(Yasli)
add_subdirectory(WinWrappers)
add_subdirectory(D3D_Tools)

set(gtest_force_shared_crt ON CACHE BOOL "")
add_subdirectory(googletest)

add_subdirectory(EverydayTools)
add_subdirectory(EverydayToolsTests)
add_subdirectory(FreeType)
