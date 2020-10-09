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

IF(HAVE_SALOME)
  SET(SUIT_INCLUDE ${SALOME_GUI_INCLUDE})
  MESSAGE(STATUS "SUIT_INCLUDE: ${SALOME_GUI_INCLUDE}")
  SET(SUIT_LIB_DIR "${GUI_ROOT_DIR}/lib/salome")
ELSE(HAVE_SALOME) 
  SET(SUIT_DIR $ENV{SUIT_DIR})
  IF(EXISTS ${SUIT_DIR})

    MESSAGE(STATUS "SUIT found at ${SUIT_DIR}")
    IF(WIN32)
        SET(SUIT_LIB_DIR ${SUIT_DIR}/lib)
    ElSE()
        SET(SUIT_LIB_DIR ${SUIT_DIR}/lib/salome)
    ENDIF()

    FIND_LIBRARY(Event Event ${SUIT_LIB_DIR})
    FIND_LIBRARY(PyConsole PyConsole ${SUIT_LIB_DIR})
    FIND_LIBRARY(PyInterp PyInterp ${SUIT_LIB_DIR})
    FIND_LIBRARY(qtx qtx ${SUIT_LIB_DIR})
    FIND_LIBRARY(suit suit ${SUIT_LIB_DIR})
    FIND_LIBRARY(ViewerData ViewerData ${SUIT_LIB_DIR})

    IF(WIN32)
        SET(SUIT_INCLUDE ${SUIT_DIR}/include)
    ElSE()
        SET(SUIT_INCLUDE ${SUIT_DIR}/include/salome)
    ENDIF()
  ENDIF(EXISTS ${SUIT_DIR})
ENDIF(HAVE_SALOME)

FILE(TO_CMAKE_PATH ${SUIT_LIB_DIR} SUIT_LIB_DIR)
FILE(TO_CMAKE_PATH ${SUIT_INCLUDE} SUIT_INCLUDE)
