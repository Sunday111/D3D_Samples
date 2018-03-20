cmake_minimum_required(VERSION 3.5.1)

# Warning level
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
else()
  message(ERROR "Unknwon compiler")
endif()

#definitions
if(MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

#compile options
if(MSVC)
  add_compile_options("/std:c++latest")
else()
  message(ERROR "Unknown compiler")
endif()