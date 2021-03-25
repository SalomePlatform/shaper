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

INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    ConstructionPlugin.h
    ConstructionPlugin_Plugin.h
    ConstructionPlugin_Point.h
    ConstructionPlugin_Axis.h
    ConstructionPlugin_Plane.h
    ConstructionPlugin_Validators.h
)

SET(PROJECT_SOURCES
    ConstructionPlugin_Plugin.cpp
    ConstructionPlugin_Point.cpp
    ConstructionPlugin_Axis.cpp
    ConstructionPlugin_Plane.cpp
    ConstructionPlugin_Validators.cpp
)

SET(XML_RESOURCES
  plugin-Construction.xml
  point_widget.xml
  axis_widget.xml
  plane_widget.xml
)

SET(TEXT_RESOURCES
  ConstructionPlugin_msg_en.ts
  ConstructionPlugin_msg_fr.ts
)

SET(PROJECT_LIBRARIES
    Config
    ModelAPI
    GeomAPI
    GeomAlgoAPI
    GeomDataAPI
)

SOURCE_GROUP ("XML Files" FILES ${XML_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

ADD_DEFINITIONS(-DCONSTRUCTIONPLUGIN_EXPORTS)
ADD_LIBRARY(ConstructionPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})
TARGET_LINK_LIBRARIES(ConstructionPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ConstructionPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Construction)

INCLUDE_DIRECTORIES(
  ../Config
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../Events
  ../GeomDataAPI
)

include(tests.set)

ADD_UNIT_TESTS(${TEST_NAMES})

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/ConstructionPlugin")
  
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
