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

SET(TEXT_RESOURCES
	PythonAddons_msg_en.ts
	PythonAddons_msg_fr.ts
)

# configuration
IF(${HAVE_SALOME})
  SET(ADDONS_FOLDER_PATH ../../../../bin/salome)
ELSE(${HAVE_SALOME})
  SET(ADDONS_FOLDER_PATH ../addons)
ENDIF(${HAVE_SALOME})

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/addons_Features.xml.in"
  "${CMAKE_CURRENT_BINARY_DIR}/addons_Features.xml"
  )

INSTALL(FILES addons_Features.py DESTINATION ${SHAPER_INSTALL_ADDONS})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/addons_Features.xml DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

INSTALL(FILES __init__.py DESTINATION ${SHAPER_INSTALL_ADDONS})
INSTALL(DIRECTORY macros DESTINATION ${SHAPER_INSTALL_ADDONS})

INSTALL(DIRECTORY macros/rectangle/icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Addons)
INSTALL(DIRECTORY macros/compoundVertices/icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Addons)
INSTALL(DIRECTORY macros/importParameters/icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Addons)
INSTALL(DIRECTORY macros/fibreNeutre/icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Addons)
INSTALL(FILES ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

INCLUDE(UnitTest)

include(tests.set)

ADD_UNIT_TESTS(${TEST_NAMES})

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/PythonAddons")

  install(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)
  install(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})

  set(TMP_TESTS_NAMES)
  foreach(tfile ${TEST_NAMES})
    list(APPEND TMP_TESTS_NAMES "Test/${tfile}")
  endforeach(tfile ${TEST_NAMES})

  install(FILES ${TMP_TESTS_NAMES} DESTINATION ${TEST_INSTALL_DIRECTORY})
endif(${HAVE_SALOME})
