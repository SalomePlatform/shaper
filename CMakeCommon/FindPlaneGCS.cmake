# - Try to  find PlaneGCS (sketcher part of FreeCAD)
# Once done this will define
#
#  PLANEGCS_FOUND - system has PlaneGCS
#  PLANEGCS_INCLUDE_DIRS - the Plaine include directory
#  PLANEGCS_LIBRARIES - Link these to use PlaneGCS

#=============================================================================
## Copyright (C) 2014-20xx CEA/DEN, EDF R&D
#=============================================================================

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