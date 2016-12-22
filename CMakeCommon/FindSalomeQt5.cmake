# Copyright (C) 2013-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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
# Author: Adrien Bruneton
#

# Qt5 detection for Salome
#
#  !! Please read the generic detection procedure in SalomeMacros.cmake !!
#

# Qt5_FIND_COMPONENTS variable is set here to prevent ParaView to reset list of Qt5 modules,
# just to avoid extra dependencies

SET(QT_INCLUDES)
SET(QT_DEFINITIONS)
SET(QT_LIBRARIES)
# Find Qt5Core to get Qt version
SET(Qt5_FIND_COMPONENTS QtCore)
SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS(Qt5Core Qt5Core_INCLUDE_DIRS 1 ENVVAR QT5_ROOT_DIR)
IF(Qt5Core_FOUND)
  LIST(APPEND QT_INCLUDES    ${Qt5Core_INCLUDE_DIRS})
  LIST(APPEND QT_DEFINITIONS ${Qt5Core_DEFINITIONS})
  LIST(APPEND QT_LIBRARIES   ${Qt5Core_LIBRARIES})
  SET(QT_VERSION "${Qt5Core_VERSION}")
ENDIF()

SET(Qt5_FIND_COMPONENTS Gui Widgets Network Xml OpenGL PrintSupport Help Test Sql Sensors Positioning Quick Qml Multimedia MultimediaWidgets WebChannel UiTools)

IF ("${QT_VERSION}" VERSION_LESS "5.6.0")
  LIST(APPEND Qt5_FIND_COMPONENTS WebKit WebKitWidgets)
ELSE()
  LIST(APPEND Qt5_OPTIONAL_COMPONENTS WebEngine WebEngineWidgets)
ENDIF()

IF(NOT WIN32)
  LIST(APPEND Qt5_FIND_COMPONENTS X11Extras)
ENDIF()

FOREACH(_Qt5_COMPONENT_ ${Qt5_FIND_COMPONENTS} ${Qt5_OPTIONAL_COMPONENTS})
  SET(_Qt5_COMPONENT Qt5${_Qt5_COMPONENT_})
  LIST(FIND Qt5_OPTIONAL_COMPONENTS ${_Qt5_COMPONENT_} idx)
  IF(${idx} GREATER -1)
   SET(Salome${_Qt5_COMPONENT}_FIND_QUIETLY TRUE)
  ENDIF()
  SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS(${_Qt5_COMPONENT} ${_Qt5_COMPONENT}_INCLUDE_DIRS 1 ENVVAR QT5_ROOT_DIR)
  LIST(APPEND QT_INCLUDES    ${${_Qt5_COMPONENT}_INCLUDE_DIRS})
  LIST(APPEND QT_DEFINITIONS ${${_Qt5_COMPONENT}_DEFINITIONS})
  LIST(APPEND QT_LIBRARIES   ${${_Qt5_COMPONENT}_LIBRARIES})
ENDFOREACH()

FIND_PACKAGE(Qt5LinguistTools)
GET_TARGET_PROPERTY(QT_LRELEASE_EXECUTABLE Qt5::lrelease LOCATION)
MARK_AS_ADVANCED(QT_LRELEASE_EXECUTABLE)

GET_FILENAME_COMPONENT(QT_BINARY_DIR ${QT_LRELEASE_EXECUTABLE} DIRECTORY)
MARK_AS_ADVANCED(QT_BINARY_DIR)

# This is only needed to correctly detect Qt help generator tool, to workaround an error 
# coming from ParaView detection procedure
FIND_PROGRAM(QT_HELP_GENERATOR
    qhelpgenerator
    PATHS "${QT_BINARY_DIR}"
    NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH
    DOC "qhelpgenerator used to compile Qt help project files")
MARK_AS_ADVANCED(QT_HELP_GENERATOR)

IF(Qt5Core_FOUND) 
  SALOME_ACCUMULATE_HEADERS(QT_INCLUDES)
  SALOME_ACCUMULATE_ENVIRONMENT(PATH ${LINGUIST_PATH})
  SALOME_ACCUMULATE_ENVIRONMENT(LD_LIBRARY_PATH ${Qt5Core_LIBRARIES})
  GET_FILENAME_COMPONENT(Qt5_DIR ${Qt5Core_DIR} PATH)
  FILE(TO_NATIVE_PATH "${Qt5_DIR}/Qt5" Qt5_DIR)
  SET(SalomeQt5_COMPONENTS ${Qt5_FIND_COMPONENTS})
  MESSAGE(STATUS "Qt version is ${QT_VERSION}")
ENDIF()
