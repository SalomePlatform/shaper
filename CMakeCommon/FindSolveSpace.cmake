# - Try to  find SolveSpace
# Once done this will define
#
#  SOLVESPACE_FOUND - system has SolveSpace
#  SOLVESPACE_INCLUDE_DIRS - the SolveSpace include directory
#  SOLVESPACE_LIBRARIES - Link these to use SolveSpace

#=============================================================================
## Copyright (C) 2014-20xx CEA/DEN, EDF R&D
#=============================================================================

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