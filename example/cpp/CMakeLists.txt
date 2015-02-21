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

set (CMAKE_VERBOSE_MAKEFILE true)

if (WIN32)
  if (${CMAKE_GENERATOR} MATCHES "Visual Studio 11 *" OR ${CMAKE_GENERATOR} MATCHES "Visual Studio 12 *")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  endif ()
endif ()

file(GLOB SOURCE_FILES  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(example ${SOURCE_FILES})

install (TARGETS example RUNTIME DESTINATION bin)
