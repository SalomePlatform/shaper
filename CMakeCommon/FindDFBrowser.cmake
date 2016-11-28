# - Try to  find DFBrowser
# Once done this will define
#
#  DBROWSER_FOUND - system has DFBrowser
#  DFBROWSER_INCLUDE_DIRS - the DFBrowser include directory
#  DFBROWSER_LIBRARIES - Link these to use DFBrowser

#=============================================================================
## Copyright (C) 2014-20xx CEA/DEN, EDF R&D
#=============================================================================

IF(NOT DFBROWSER_FIND_QUIETLY)
  MESSAGE(STATUS "Try to find DFBrowser at $ENV{DFBROWSER_ROOT_DIR}...")
ENDIF()

SET(DFBROWSER_ROOT_DIR $ENV{DFBROWSER_ROOT_DIR} CACHE PATH "Path to the DFBrowser directory")
IF(EXISTS ${DFBROWSER_ROOT_DIR})
  SET(DFBROWSER_INCLUDE_DIR "${DFBROWSER_ROOT_DIR}/include")
  SET(DFBROWSER_LIB_DIR "${DFBROWSER_ROOT_DIR}/lib")

  FIND_LIBRARY(DFBrowserAPI DFBrowserAPI "${DFBROWSER_LIB_DIR}")
  SET(DFBrowserAPI ${DFBrowserAPI})

  MESSAGE(STATUS "DFBrowser found at ${DFBROWSER_ROOT_DIR}")
  MESSAGE(STATUS "DFBrowser include dir at ${DFBROWSER_INCLUDE_DIR}")
  MESSAGE(STATUS "DFBrowser lib dir at ${DFBROWSER_LIB_DIR}")
ENDIF(EXISTS ${DFBROWSER_ROOT_DIR})
