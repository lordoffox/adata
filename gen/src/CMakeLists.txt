#
# This file is part of the CMake build system for adata gen
#
# CMake auto-generated configuration options.
# Do not check in modified versions of this file.
#
# Copyright (c) 2014-2014 lordoffox (QQ:99643412 lordoffox@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

cmake_minimum_required (VERSION 2.8.6 FATAL_ERROR)
project (adata_gen)

# The version number.
set (ADATA_VERSION_MAJOR 0)
set (ADATA_VERSION_MINOR 1)

option (STL_USE_STATIC_LIBS "Build stl runtime static" OFF)
option (ADATA_STATIC "Build adata_gen runtime static" OFF)

if (UNIX)
  set (STL_USE_STATIC_LIBS,OFF)
  set (ADATA_STATIC,OFF)
endif ()

if (WIN32)
  set (WINVER "0x0501" CACHE STRING "Windows version maro. Default is 0x0501 - winxp, user can reset")
  add_definitions (-D_WIN32_WINNT=${WINVER})
endif ()

if (MSVC)
  add_definitions (-D__CRT_SECURE_NO_WARNINGS)
endif()

# Set glibc.
if (ADATA_STATIC)
  set (GLIBC_INCLUDEDIR "" CACHE PATH "Path to glibc include directory")
  set (GLIBC_LIBRARYDIR "" CACHE PATH "Path to glibc libraries directory")
  if (GLIBC_INCLUDEDIR)
    include_directories (${GLIBC_INCLUDEDIR})
  endif ()
  if (GLIBC_LIBRARYDIR)
    link_directories (${GLIBC_LIBRARYDIR})
  endif ()
endif ()

# Add the source and build tree to the search path for include gce header files.
include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_BINARY_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../include/)
#stl libraries option
if(STL_USE_STATIC_LIBS)
  if(MSVC)
    add_definitions (-D_STATIC_CPPLIB)
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELWITHDEBINFO
        )
    foreach(CompilerFlag ${CompilerFlags})
      string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
  endif ()
  if(CMAKE_COMPILER_IS_GNUCXXX)
  endif ()
endif ()

# Boost libraries search.
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)
if (ADATA_STATIC)
  set (Boost_USE_STATIC_RUNTIME ON)
else ()
  set (Boost_USE_STATIC_RUNTIME OFF)
endif ()
if (BOOST_ROOT)
  set (Boost_NO_SYSTEM_PATHS ON)
endif ()

find_package (Boost 1.55.0 REQUIRED program_options)

mark_as_advanced (Boost_DIR)
if (WIN32)
  mark_as_advanced (Boost_LIB_DIAGNOSTIC_DEFINITIONS)
endif ()

include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})

set (CMAKE_VERBOSE_MAKEFILE true)

file(GLOB_RECURSE SOURCE_FILES  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(adata_gen ${SOURCE_FILES})

# Build a CPack driven installer package.
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "${ADATA_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${ADATA_VERSION_MINOR}")
set (CPACK_PACKAGE_CONTACT "QQ:99643412 lordoffox: lordoffox@gmail.com")
include (CPack)
