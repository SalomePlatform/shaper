# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

IF (WIN32)
  FIND_PROGRAM(DOXYGEN_EXECUTABLE Doxygen)
ELSE (WIN32)
  FIND_PROGRAM(DOXYGEN_EXECUTABLE doxygen)
ENDIF(WIN32)

IF(HAVE_SALOME)
  SET(EXCLUDE_DOC_DIR "*/AppElements/* */OpenParts/*")
ELSE(HAVE_SALOME)
  SET(EXCLUDE_DOC_DIR "*/Shaper/*")
ENDIF(HAVE_SALOME)

CONFIGURE_FILE(doxyfile.in
               "${CMAKE_CURRENT_BINARY_DIR}/doxyfile"
               IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(dev_docs
    COMMAND "${DOXYGEN_EXECUTABLE}"
    COMMAND "${CMAKE_COMMAND}" --build "${PROJECT_BINARY_DIR}" --target install --config ${CMAKE_BUILD_TYPE}
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/doc/tui"
)

INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tui/ DESTINATION ${SHAPER_INSTALL_TUI_DOC} OPTIONAL)
