cmake_minimum_required(VERSION 3.5.1)

add_subdirectory(CMakeModules)
add_subdirectory(Yasli)

####################################################
# Google tests
if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "")
endif()
add_subdirectory(googletest)
set_target_properties(gtest PROPERTIES FOLDER "Submodules/Tests/gtest")
set_target_properties(gtest_main PROPERTIES FOLDER "Submodules/Tests/gtest")
set_target_properties(gmock PROPERTIES FOLDER "Submodules/Tests/gtest")
set_target_properties(gmock_main PROPERTIES FOLDER "Submodules/Tests/gtest")

####################################################
# Everyday tools library
add_subdirectory(EverydayTools)
set_target_properties(EverydayToolsView PROPERTIES FOLDER "Submodules")
set_target_properties(EverydayToolsTests PROPERTIES FOLDER "Submodules/Tests")

####################################################
# CppReflection Library
add_subdirectory(CppReflection)
set_target_properties(CppReflection PROPERTIES FOLDER "Submodules")
set_target_properties(CppReflectionTests PROPERTIES FOLDER "Submodules/Tests")

####################################################
# WinWrappers
add_subdirectory(WinWrappers)
set_target_properties(WinWrappersView PROPERTIES FOLDER "Submodules")

####################################################
# FreeType fonts library
add_subdirectory(FreeType)
