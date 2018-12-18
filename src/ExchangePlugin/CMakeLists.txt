## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

INCLUDE(Common)
INCLUDE(UnitTest)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/ModelHighAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
                    ${PROJECT_SOURCE_DIR}/src/XAO
)

SET(PROJECT_HEADERS
    ExchangePlugin.h
    ExchangePlugin_Plugin.h
    ExchangePlugin_ImportFeature.h
    ExchangePlugin_ExportFeature.h
    ExchangePlugin_Validators.h
    ExchangePlugin_Tools.h
    ExchangePlugin_Dump.h
)

SET(PROJECT_SOURCES
    ExchangePlugin_Plugin.cpp
    ExchangePlugin_ImportFeature.cpp
    ExchangePlugin_ExportFeature.cpp
    ExchangePlugin_Validators.cpp
    ExchangePlugin_Tools.cpp
    ExchangePlugin_Dump.cpp
)

SET(XML_RESOURCES
    export_widget.xml
    plugin-Exchange.xml
)

SET(TEXT_RESOURCES
    ExchangePlugin_msg_en.ts
)

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    ModelHighAPI
    GeomAPI
    GeomAlgoAPI
    XAOShaper
)
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

ADD_DEFINITIONS(-DEXCHANGEPLUGIN_EXPORTS)
ADD_LIBRARY(ExchangePlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})

TARGET_LINK_LIBRARIES(ExchangePlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ExchangePlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Exchange)
INSTALL(FILES ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

ADD_UNIT_TESTS(TestImport.py
               TestExport.py
               Test2290.py
               Test2459.py
               TestExportToXAOWithFields.py
               TestExportToXAOWithGroupNotUpdated.py
               TestExport_FiniteValidator.py
)
