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

INCLUDE(Common)
INCLUDE(UnitTest)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/Locale
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/ModelHighAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomValidators
                    ${PROJECT_SOURCE_DIR}/src/XAO
                    ${PROJECT_SOURCE_DIR}/src/ConstructionPlugin
                    ${PROJECT_SOURCE_DIR}/src/PartSetPlugin
                    ${OpenCASCADE_INCLUDE_DIR}
)

SET(PROJECT_HEADERS
    ExchangePlugin.h
    ExchangePlugin_Plugin.h
    ExchangePlugin_ImportFeature.h
    ExchangePlugin_ExportFeature.h
    ExchangePlugin_Validators.h
    ExchangePlugin_Tools.h
    ExchangePlugin_Dump.h
    ExchangePlugin_ImportPart.h
    ExchangePlugin_ExportPart.h
    ExchangePlugin_Import.h
)

SET(PROJECT_SOURCES
    ExchangePlugin_Plugin.cpp
    ExchangePlugin_ImportFeature.cpp
    ExchangePlugin_ExportFeature.cpp
    ExchangePlugin_Validators.cpp
    ExchangePlugin_Tools.cpp
    ExchangePlugin_Dump.cpp
    ExchangePlugin_ImportPart.cpp
    ExchangePlugin_ExportPart.cpp
    ExchangePlugin_Import.cpp
)

SET(XML_RESOURCES
    export_widget.xml
    plugin-Exchange.xml
)

SET(TEXT_RESOURCES
    ExchangePlugin_msg_en.ts
    ExchangePlugin_msg_fr.ts
)

SOURCE_GROUP ("XML Files" FILES ${XML_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

SET(PROJECT_LIBRARIES
    Events
    Config
    Locale
    ModelAPI
    ModelHighAPI
    GeomAPI
    GeomAlgoAPI
    GeomValidators
    XAOShaper
)

# default dump approaches (will be set if not initialized)
SET(PYTHONDUMP_NAMING ON  CACHE BOOL "Dump named references to shapes")
SET(PYTHONDUMP_GEO    ON  CACHE BOOL "Dump references to shapes by the geometric properties")
SET(PYTHONDUMP_WEAK   OFF CACHE BOOL "Dump weak named references to shapes")

ADD_DEFINITIONS(-DEXCHANGEPLUGIN_EXPORTS)
IF(${PYTHONDUMP_NAMING})
  ADD_DEFINITIONS(-DEXCHANGEPLUGIN_DUMP_NAMING)
ENDIF()
IF(${PYTHONDUMP_GEO})
  ADD_DEFINITIONS(-DEXCHANGEPLUGIN_DUMP_GEO)
ENDIF()
IF(${PYTHONDUMP_WEAK})
  ADD_DEFINITIONS(-DEXCHANGEPLUGIN_DUMP_WEAK)
ENDIF()
ADD_LIBRARY(ExchangePlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})

TARGET_LINK_LIBRARIES(ExchangePlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ExchangePlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Exchange)
INSTALL(FILES ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

include(tests.set)

ADD_UNIT_TESTS(${TEST_NAMES})

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/ExchangePlugin")
  
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
