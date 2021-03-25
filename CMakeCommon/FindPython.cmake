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

UNSET(PYTHON_INCLUDE_DIRS)
UNSET(PYTHON_LIBRARIES)

IF(WIN32)
    IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
      SET(PYTHON_LIBRARY_FILE "python36_d.lib")
    ElSE()
      SET(PYTHON_LIBRARY_FILE "python36.lib")
    ENDIF()
ElSE()
    SET(PYTHON_LIBRARY_FILE "libpython3.6.so")
ENDIF()

IF(DEFINED ENV{PYTHONHOME})
  IF(WIN32)
    FILE(TO_CMAKE_PATH "$ENV{PYTHONHOME}/libs/${PYTHON_LIBRARY_FILE}" PATH_PYTHON_LIBRARY)
  ElSE()
    FILE(TO_CMAKE_PATH "$ENV{PYTHONHOME}/lib/${PYTHON_LIBRARY_FILE}" PATH_PYTHON_LIBRARY)
  ENDIF()
  SET(PYTHON_LIBRARY ${PATH_PYTHON_LIBRARY} CACHE FILEPATH "path to the python library" FORCE)
ENDIF()

IF(DEFINED ENV{PYTHON_INCLUDE})
  FILE(TO_CMAKE_PATH $ENV{PYTHON_INCLUDE} PATH_PYTHON_INCLUDE_DIR)
  SET(PYTHON_INCLUDE_DIR ${PATH_PYTHON_INCLUDE_DIR} CACHE PATH "path to where Python.h is found" FORCE)
ELSE()
  IF(WIN32)
    IF(DEFINED ENV{PYTHON_ROOT_DIR})
     SET(PYTHON_ROOT_DIR $ENV{PYTHON_ROOT_DIR})
     SET(PYTHON_INCLUDE_DIR ${PYTHON_ROOT_DIR} CACHE PATH "path to where Python.h is found" FORCE)
    ENDIF()
  ENDIF()
ENDIF()

FIND_PACKAGE(PythonInterp REQUIRED)
FIND_PACKAGE(PythonLibs REQUIRED)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR})