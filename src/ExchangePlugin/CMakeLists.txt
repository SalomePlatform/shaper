## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(UnitTest)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
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
    plugin-Exchange.xml
)

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    GeomAPI
    GeomAlgoAPI
)

ADD_DEFINITIONS(-DEXCHANGEPLUGIN_EXPORTS)
ADD_LIBRARY(ExchangePlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(ExchangePlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ExchangePlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Exchange)

ADD_UNIT_TESTS(TestImport.py TestExport.py)

CONFIGURE_FILE(Test/Data/solid.brep Data/solid.brep COPYONLY)
CONFIGURE_FILE(Test/Data/solid.brp Data/solid.brp COPYONLY)
CONFIGURE_FILE(Test/Data/screw.step Data/screw.step COPYONLY)
CONFIGURE_FILE(Test/Data/screw.stp Data/screw.stp COPYONLY)
CONFIGURE_FILE(Test/Data/bearing.iges Data/bearing.iges COPYONLY)
CONFIGURE_FILE(Test/Data/bearing.igs Data/bearing.igs COPYONLY)
