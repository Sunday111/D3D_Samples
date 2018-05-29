cmake_minimum_required(VERSION 3.5.1)

add_subdirectory(Yasli)
add_subdirectory(WinWrappers)

####################################################
# Google tests
set(gtest_force_shared_crt ON CACHE BOOL "")
add_subdirectory(googletest)
set_target_properties(gtest PROPERTIES FOLDER "Tests/gtest")
set_target_properties(gtest_main PROPERTIES FOLDER "Tests/gtest")
set_target_properties(gmock PROPERTIES FOLDER "Tests/gtest")
set_target_properties(gmock_main PROPERTIES FOLDER "Tests/gtest")

####################################################
# Everyday tools library
add_subdirectory(EverydayTools)

####################################################
# Everyday tools library tests
add_subdirectory(EverydayToolsTests)
set_target_properties(EverydayToolsTests PROPERTIES FOLDER "Tests")

####################################################
# FreeType fonts library
add_subdirectory(FreeType)
