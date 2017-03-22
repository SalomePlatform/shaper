# - Try to  find TInspector
# Once done this will define
#
#  TInspector - system has OCCT tool inspector
#  TINSPECTOR_INCLUDE_DIRS - the tool inspector include directory
#  TINSPECTOR_LIBRARIES - Link these to use TInspector

#=============================================================================
## Copyright (C) 2014-20xx CEA/DEN, EDF R&D
#=============================================================================

SET(TINSPECTOR_ROOT_DIR $ENV{TINSPECTOR_ROOT_DIR} CACHE PATH "Path to the TInspector directory")
IF(EXISTS ${TINSPECTOR_ROOT_DIR})
  SET(TINSPECTOR_INCLUDE_DIR "${TINSPECTOR_ROOT_DIR}/inc")
  SET(TINSPECTOR_LIB_DIR "${TINSPECTOR_ROOT_DIR}/lib")

  FIND_LIBRARY(TKTInspector TKTInspector "${TINSPECTOR_LIB_DIR}")
  IF(TKTInspector)
    SET(TKTInspector ${TKTInspector})
    MESSAGE(STATUS "TInspector is found at ${TINSPECTOR_ROOT_DIR}")
    MESSAGE(STATUS "TInspector include dir is ${TINSPECTOR_INCLUDE_DIR}")
    MESSAGE(STATUS "TInspector lib dir is ${TINSPECTOR_LIB_DIR}")

    FIND_LIBRARY(TKVInspector TKVInspector "${TINSPECTOR_LIB_DIR}")
    SET(TKVInspector ${TKVInspector})

    FIND_LIBRARY(DFBrowserAPI DFBrowserAPI "${TINSPECTOR_LIB_DIR}")
    SET(DFBrowserAPI ${DFBrowserAPI})

    FIND_LIBRARY(TKDFTreeModel TKDFTreeModel "${TINSPECTOR_LIB_DIR}")
    SET(TKDFTreeModel ${TKDFTreeModel})

  ENDIF(TKTInspector)

ENDIF(EXISTS ${TINSPECTOR_ROOT_DIR})
