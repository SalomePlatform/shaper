# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

# - Try to  find TInspector
# Once done this will define
#
#  TInspector - system has OCCT tool inspector
#  TINSPECTOR_INCLUDE_DIRS - the tool inspector include directory
#  TINSPECTOR_LIBRARIES - Link these to use TInspector

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

    FIND_LIBRARY(TKTInspectorAPI TKTInspectorAPI "${TINSPECTOR_LIB_DIR}")
    SET(TKTInspectorAPI ${TKTInspectorAPI})

    FIND_LIBRARY(TKTreeModel TKTreeModel "${TINSPECTOR_LIB_DIR}")
    SET(TKTreeModel ${TKTreeModel})

    FIND_LIBRARY(TKVInspector TKVInspector "${TINSPECTOR_LIB_DIR}")
    SET(TKVInspector ${TKVInspector})

  ENDIF(TKTInspector)

ENDIF(EXISTS ${TINSPECTOR_ROOT_DIR})
