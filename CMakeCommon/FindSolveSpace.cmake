# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

# - Try to  find SolveSpace
# Once done this will define
#
#  SOLVESPACE_FOUND - system has SolveSpace
#  SOLVESPACE_INCLUDE_DIRS - the SolveSpace include directory
#  SOLVESPACE_LIBRARIES - Link these to use SolveSpace

IF(NOT SOLVESPACE_FIND_QUIETLY)
  MESSAGE(STATUS "Try to find SolveSpace at $ENV{SOLVESPACE_ROOT_DIR}...")
ENDIF()

FIND_PATH(SOLVESPACE_INCLUDE_DIR NAMES slvs.h
  HINTS ENV SOLVESPACE_ROOT_DIR
  PATH_SUFFIXES include
)

FIND_LIBRARY(SOLVESPACE_LIBRARY NAMES slvs
  HINTS ENV SOLVESPACE_ROOT_DIR
  PATH_SUFFIXES lib
)

SET(SOLVESPACE_INCLUDE_DIRS ${SOLVESPACE_INCLUDE_DIR})
SET(SOLVESPACE_LIBRARIES ${SOLVESPACE_LIBRARY})

# handle the QUIETLY and REQUIRED arguments and set SOLVESPACE_FOUND to TRUE
# if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SolveSpace
                                  REQUIRED_VARS SOLVESPACE_LIBRARY SOLVESPACE_INCLUDE_DIR)

MARK_AS_ADVANCED(SOLVESPACE_LIBRARY SOLVESPACE_INCLUDE_DIR)