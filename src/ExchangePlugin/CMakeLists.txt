## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
                    ${CAS_INCLUDE_DIRS}
) 

SET(PROJECT_HEADERS
    ExchangePlugin.h
    ExchangePlugin_Plugin.h
    ExchangePlugin_ImportFeature.h
    ExchangePlugin_Validators.h
    ExchangePlugin_BREPImport.h
    ExchangePlugin_STEPImport.h
)
 
SET(PROJECT_SOURCES
    ExchangePlugin_Plugin.cpp
    ExchangePlugin_ImportFeature.cpp
    ExchangePlugin_Validators.cpp
    ExchangePlugin_BREPImport.cpp
    ExchangePlugin_STEPImport.cpp
)

SET(XML_RESOURCES
    plugin-Exchange.xml
)

SET(PROJECT_LIBRARIES
    Events
    ModelAPI
    Config
    GeomAPI
    ${CAS_TKBRep}
    ${CAS_TKSTEP}
    ${CAS_TKSTEPBase}
    ${CAS_TKXSBase}
    ${CAS_TKCAF}
    ${CAS_TKLCAF}
    ${CAS_OCAF}
    ${CAS_TKCAF}
    ${CAS_SHAPE}
    ${CAS_TKBO}
)

ADD_DEFINITIONS(-DEXCHANGEPLUGIN_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(ExchangePlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(ExchangePlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ExchangePlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
