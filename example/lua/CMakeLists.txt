#
# This file is part of the CMake build system for adatac
#
# CMake auto-generated configuration options.
# Do not check in modified versions of this file.
#
# Copyright (c) 2014-2015 lordoffox (QQ:99643412 lordoffox@gmail.com)
# Copyright (c) 2015 Nous Xiong (QQ:348944179 348944179@qq.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required (VERSION 2.8.6 FATAL_ERROR)
project (example)

# Add library.
macro (add_library library_name name)
  string (TOUPPER ${library_name} add_library_upper_name)
  set (${add_library_upper_name}_LIBRARYDIR "" CACHE PATH "Path to ${library_name} libraries directory")
  set (${add_library_upper_name}_INCLUDEDIR "" CACHE PATH "Path to ${library_name} include directory")
  include_directories (${${add_library_upper_name}_INCLUDEDIR})

  string (TOUPPER ${library_name} set_lib_upper_name)
  string (TOLOWER ${name} set_lib_lower_name)
  find_library (${name}_LIBRARY_DEBUG NAMES ${set_lib_lower_name}_debug PATHS "${${set_lib_upper_name}_LIBRARYDIR}" NO_DEFAULT_PATH)
  find_library (${name}_LIBRARY_RELEASE NAMES ${set_lib_lower_name} PATHS "${${set_lib_upper_name}_LIBRARYDIR}" NO_DEFAULT_PATH)

  if (${name}_LIBRARY_DEBUG AND ${name}_LIBRARY_RELEASE)
    if (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      set (${name}_LIBRARIES optimized ${${name}_LIBRARY_RELEASE} debug ${${name}_LIBRARY_DEBUG})
    else ()
      set (${name}_LIBRARIES ${${name}_LIBRARY_RELEASE})
    endif ()
  endif ()

  if (${name}_LIBRARY_RELEASE AND NOT ${name}_LIBRARY_DEBUG)
    set (${name}_LIBRARIES ${${name}_LIBRARY_RELEASE})
  endif ()

  if (${name}_LIBRARY_DEBUG AND NOT ${name}_LIBRARY_RELEASE)
    set (${name}_LIBRARIES ${${name}_LIBRARY_DEBUG})
  endif ()
endmacro (add_library)

if (WIN32)
  set (WINVER "0x0501" CACHE STRING "Windows version maro. Default is 0x0501 - winxp, user can reset")
  add_definitions (-D_WIN32_WINNT=${WINVER})
endif ()

if (MSVC)
  add_definitions (-D__CRT_SECURE_NO_WARNINGS)
endif()

# Add the source and build tree to the search path for include header files.
include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_BINARY_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../cpp)
include_directories (${PROJECT_SOURCE_DIR}/generated)

add_definitions (-DSOURCE_DIR="${PROJECT_SOURCE_DIR}")

set (CMAKE_VERBOSE_MAKEFILE true)

add_library(lua Lua)

if (WIN32)
  if (${CMAKE_GENERATOR} MATCHES "Visual Studio 11 *" OR ${CMAKE_GENERATOR} MATCHES "Visual Studio 12 *")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  endif ()
endif ()

file(GLOB SOURCE_FILES  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(example ${SOURCE_FILES})
target_link_libraries (example ${Lua_LIBRARIES})

install (TARGETS example RUNTIME DESTINATION bin)
