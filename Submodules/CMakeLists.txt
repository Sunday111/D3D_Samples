cmake_minimum_required(VERSION 3.5.1)

add_subdirectory(CMakeModules)
add_subdirectory(Yasli)
add_subdirectory(WinWrappers)

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
set_target_properties(EverydayTools_ PROPERTIES FOLDER "Submodules")

####################################################
# Everyday tools library tests
add_subdirectory(EverydayToolsTests)
set_target_properties(EverydayToolsTests PROPERTIES FOLDER "Submodules/Tests")

####################################################
# FreeType fonts library
add_subdirectory(FreeType)
