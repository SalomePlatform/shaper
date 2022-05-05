# - Sphinx detection
#
# Output variables:
#   SPHINX_EXECUTABLE - path to the Sphinx executable
#   SPHINX_PYTHONPATH - path to the Sphinx Python modules
#
###########################################################################
# Copyright (C) 2013-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

FIND_PROGRAM(SPHINX_EXECUTABLE
	     NAMES sphinx-build sphinx-build-${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}
	     PATH_SUFFIXES Scripts)
FIND_PROGRAM(SPHINX_APIDOC_EXECUTABLE
             NAMES sphinx-apidoc sphinx-apidoc-${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}
	     PATH_SUFFIXES Scripts)

# Get root dir locally, going up two levels from the exec:
GET_FILENAME_COMPONENT(_tmp_ROOT_DIR "${SPHINX_EXECUTABLE}" PATH)
GET_FILENAME_COMPONENT(_tmp_ROOT_DIR "${_tmp_ROOT_DIR}" PATH)
IF(WIN32)
  SET(SPHINX_PYTHONPATH "${_tmp_ROOT_DIR}/lib/site-packages")
ELSE()
  SET(SPHINX_PYTHONPATH "${_tmp_ROOT_DIR}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages")
ENDIF()

# Handle the standard arguments of the find_package() command:
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sphinx REQUIRED_VARS SPHINX_EXECUTABLE SPHINX_APIDOC_EXECUTABLE)

IF(SPHINX_EXECUTABLE)
  IF(WIN32)
    # Call Sphinx through the Python executable to eliminate the problem related to starting the Sphinx's process.
    GET_FILENAME_COMPONENT(_tmp_ROOT_DIR "${SPHINX_EXECUTABLE}" PATH)
    GET_FILENAME_COMPONENT(_tmp_FILENAME "${SPHINX_EXECUTABLE}" NAME_WE)
    SET(SPHINX_EXECUTABLE "${PYTHON_EXECUTABLE}" "${_tmp_ROOT_DIR}/${_tmp_FILENAME}-script.py")
  ENDIF()

  EXECUTE_PROCESS(COMMAND ${SPHINX_EXECUTABLE} "--version" OUTPUT_VARIABLE SPHINX_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
  STRING(REGEX REPLACE ".* ([0-9.]+)$" "\\1" SPHINX_VERSION "${SPHINX_VERSION}" )
  MESSAGE(STATUS "Sphinx version is ${SPHINX_VERSION}")
  IF(SPHINX_VERSION VERSION_LESS "1.3")
    SET(SPHINX_THEME "default")
  ELSE()
    SET(SPHINX_THEME "classic")
  ENDIF()
ENDIF(SPHINX_EXECUTABLE)
