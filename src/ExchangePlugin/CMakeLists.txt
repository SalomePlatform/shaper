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

INCLUDE(Common)
INCLUDE(UnitTest)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/ModelHighAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomValidators
                    ${PROJECT_SOURCE_DIR}/src/XAO
                    ${PROJECT_SOURCE_DIR}/src/ConstructionPlugin
                    ${PROJECT_SOURCE_DIR}/src/PartSetPlugin
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

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    ModelHighAPI
    GeomAPI
    GeomAlgoAPI
    GeomValidators
    XAOShaper
)
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

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

ADD_UNIT_TESTS(
  TestImport.py
  TestExport.py
  Test2290.py
  Test2459.py
  Test18710.py
  TestExportToXAOWithFields.py
  TestExportToXAOWithGroupNotUpdated.py
  TestExport_FiniteValidator.py
  TestExportPart_Failure_1.py
  TestExportPart_Failure_2.py
  TestExportPart_Failure_3.py
  TestExportPart_FullPartSet.py
  TestExportPart_FullPart_1.py
  TestExportPart_FullPart_2.py
  TestExportPart_PartSet.py
  TestExportPart_Results_1.py
  TestExportPart_Results_2.py
  TestExportPart_Results_3.py
  TestExportPart_Results_4.py
  TestExportPart_Results_5.py
  TestExportPart_Results_6.py
  TestExportPart_Results_7.py
  TestExportPart_Results_8.py
  TestImportPart_AfterCurrent_1.py
  TestImportPart_AfterCurrent_2.py
  TestImportPart_AfterLast_1.py
  TestImportPart_AfterLast_2.py
  TestImportPart_AfterLast_3.py
  TestImportPart_AfterLast_4.py
  TestImportPart_AfterLast_5.py
  TestImportPart_AfterLast_6.py
  TestImportPart_Construction_1.py
  TestImportPart_Construction_2.py
  TestImportPart_Construction_3.py
  TestImportPart_Construction_4.py
  TestImportPart_Multiple.py
  TestImportPart_ToEmptyPart.py
  TestImportPart_ToEmptyPartSet.py
)
