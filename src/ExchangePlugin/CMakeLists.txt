## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(UnitTest)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
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
)

SET(PROJECT_SOURCES
    ExchangePlugin_Plugin.cpp
    ExchangePlugin_ImportFeature.cpp
    ExchangePlugin_ExportFeature.cpp
    ExchangePlugin_Validators.cpp
    ExchangePlugin_Tools.cpp
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
    GeomAPI
    GeomAlgoAPI
    XAO
)
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

ADD_DEFINITIONS(-DEXCHANGEPLUGIN_EXPORTS)
ADD_LIBRARY(ExchangePlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})

TARGET_LINK_LIBRARIES(ExchangePlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ExchangePlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Exchange)
INSTALL(FILES ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

ADD_UNIT_TESTS(TestImport.py TestExport.py)

SET(DATA_FILES
  solid.brep
  solid.brp
  screw.step
  screw.stp
  bearing.iges
  bearing.igs
  Box_1.brep
  test.xao
  export_ref.xao
)

FOREACH(file ${DATA_FILES})
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/Test/Data/${file}
    ${CMAKE_CURRENT_BINARY_DIR}/Data/${file}
    COPYONLY)
ENDFOREACH(file ${DATA_FIELS})

