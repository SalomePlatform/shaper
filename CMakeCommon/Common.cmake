# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0011 NEW)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

IF(WIN32)
  ## Specific definitions: EHsc to avoid exceptions-linkage unresolved symbols
  ADD_DEFINITIONS(-DWIN32 -D_WINDOWS /EHsc)
ENDIF(WIN32)

IF(UNIX)
  IF(CMAKE_COMPILER_IS_GNUCC)
    #C++11 is not supported on some platforms, disable it
    #MESSAGE(STATUS "Setting -std=c++0x flag for the gcc...")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

    #Supporting test coverage checks (gcov) in the DEBUG mode
    IF(USE_TEST_COVERAGE)
      INCLUDE(CodeCoverage)
      MESSAGE(STATUS "Setting flags for gcov support the gcc...")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
      SET(CMAKE_SHARED_LINKER_FLAGS "-lgcov")

      SETUP_TARGET_FOR_COVERAGE(test_coverage  # Name for custom target.
                                ctest          # Name of the test driver executable that runs the tests.
                                coverage)      # Name of output directory.

      SET(USE_TEST_COVERAGE False)
    ENDIF(USE_TEST_COVERAGE)

    #SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -E")
    #MESSAGE(STATUS "gcc flags are: " ${CMAKE_CXX_FLAGS})
    #MESSAGE(STATUS "linker flags are: " ${CMAKE_SHARED_LINKER_FLAGS})
  ENDIF(CMAKE_COMPILER_IS_GNUCC)
ENDIF(UNIX)

# Set highest warnings level
IF(MSVC)
  IF(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
ELSE()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
ENDIF()
