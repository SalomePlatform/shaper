# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

# - Try to  find PlaneGCS (sketcher part of FreeCAD)
# Once done this will define
#
#  PLANEGCS_FOUND - system has PlaneGCS
#  PLANEGCS_INCLUDE_DIRS - the Plaine include directory
#  PLANEGCS_LIBRARIES - Link these to use PlaneGCS

IF(NOT PLANEGCS_FIND_QUIETLY)
  MESSAGE(STATUS "Try to find PlainGCS at $ENV{PLANEGCS_ROOT_DIR}...")
ENDIF()

FIND_PATH(PLANEGCS_INCLUDE_DIR NAMES GCS.h
  HINTS ENV PLANEGCS_ROOT_DIR
  PATH_SUFFIXES include
)

FIND_PATH(PLANEGCS_CMAKE_DIR NAMES FindEigen3.cmake
  HINTS ENV PLANEGCS_ROOT_DIR
  PATH_SUFFIXES cmake
)

FIND_LIBRARY(PLANEGCS_LIBRARY NAMES PlaneGCS
  HINTS ENV PLANEGCS_ROOT_DIR
  PATH_SUFFIXES lib
)

# verify Eigen and Boost libraries are found
FILE(TO_CMAKE_PATH "$ENV{EIGEN_ROOT_DIR}/include/eigen3" EIGEN3_INCLUDE_DIR)
FILE(TO_CMAKE_PATH "$ENV{BOOST_ROOT_DIR}" BOOST_ROOT)
SET(CMAKE_MODULE_PATH "${PLANEGCS_CMAKE_DIR}" ${CMAKE_MODULE_PATH})
FIND_PACKAGE(Eigen3 3.2.5 REQUIRED)
FIND_PACKAGE(Boost COMPONENTS graph REQUIRED)

IF(Boost_FOUND AND EIGEN3_FOUND)
  SET(PLANEGCS_INCLUDE_DIRS ${PLANEGCS_INCLUDE_DIR})
  SET(PLANEGCS_LIBRARIES ${PLANEGCS_LIBRARY})
ENDIF()

# handle the QUIETLY and REQUIRED arguments and set PLANEGCS_FOUND to TRUE
# if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PlaneGCS
                                  REQUIRED_VARS PLANEGCS_LIBRARY PLANEGCS_INCLUDE_DIR)

MARK_AS_ADVANCED(PLANEGCS_LIBRARY PLANEGCS_INCLUDE_DIR)