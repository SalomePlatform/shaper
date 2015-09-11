# - Try to  find SolveSpace
# Once done this will define
#
#  SolveSpace_FOUND - system has SolveSpace
#  SolveSpace_INCLUDE_DIRS - the SolveSpace include directory
#  SolveSpace_LIBRARIES - Link these to use SolveSpace

#=============================================================================
## Copyright (C) 2014-20xx CEA/DEN, EDF R&D
#=============================================================================

SET(_SolveSpace_PATHS $ENV{SOLVESPACE_ROOT_DIR})

FIND_PATH(SolveSpace_INCLUDE_DIR NAMES slvs.h
          PATHS ${_SolveSpace_PATHS}
          PATH_SUFFIXES include)

FIND_LIBRARY(SolveSpace_LIBRARY NAMES slvs
             PATHS ${_SolveSpace_PATHS}
             PATH_SUFFIXES lib)

SET(SolveSpace_INCLUDE_DIRS ${SolveSpace_INCLUDE_DIR})
SET(SolveSpace_LIBRARIES ${SolveSpace_LIBRARY})

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SolveSpace_FOUND to TRUE
# if all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SolveSpace DEFAULT_MSG
                                  SolveSpace_LIBRARY SolveSpace_INCLUDE_DIR)

MARK_AS_ADVANCED(SolveSpace_LIBRARY SolveSpace_INCLUDE_DIR)

IF(NOT SolveSpace_FIND_QUIET)
    MESSAGE(STATUS "SolveSpace found at ${_SolveSpace_PATHS}")
ENDIF()