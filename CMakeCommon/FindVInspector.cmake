# - Try to  find VInspector
# Once done this will define
#
#  VINSPECTOR_FOUND - system has VInspector
#  VINSPECTOR_INCLUDE_DIRS - the VInspector include directory
#  VINSPECTOR_LIBRARIES - Link these to use VInspector

#=============================================================================
## Copyright (C) 2014-20xx CEA/DEN, EDF R&D
#=============================================================================

SET(VINSPECTOR_ROOT_DIR $ENV{VINSPECTOR_ROOT_DIR} CACHE PATH "Path to the VInspector directory")
IF(EXISTS $ENV{VINSPECTOR_ROOT_DIR})
  SET(VINSPECTOR_INCLUDE_DIR "$ENV{VINSPECTOR_ROOT_DIR}/include")
  SET(VINSPECTOR_LIB_DIR "$ENV{VINSPECTOR_ROOT_DIR}/lib")

  FIND_LIBRARY(VInspectorAPI VInspectorAPI "${VINSPECTOR_LIB_DIR}")
  IF(VInspectorAPI)
    SET(VInspectorAPI ${VInspectorAPI})
    MESSAGE(STATUS "VInspector found at ${VINSPECTOR_ROOT_DIR}")
    MESSAGE(STATUS "VInspector include dir at ${VINSPECTOR_INCLUDE_DIR}")
    MESSAGE(STATUS "VInspector lib dir at ${VINSPECTOR_LIB_DIR}")
  ENDIF(VInspectorAPI)

ENDIF(EXISTS $ENV{VINSPECTOR_ROOT_DIR})
