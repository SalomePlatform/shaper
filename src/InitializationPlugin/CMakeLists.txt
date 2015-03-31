
INCLUDE(Common)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
) 

SET(PROJECT_HEADERS
    InitializationPlugin.h
    InitializationPlugin_Plugin.h
    #InitializationPlugin_OriginPlanesFeature.h
)
 
SET(PROJECT_SOURCES
    InitializationPlugin_Plugin.cpp
    #InitializationPlugin_OriginPlanesFeature.cpp
)

#SET(XML_RESOURCES
#)

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
)

ADD_DEFINITIONS(-DINITIALIZATIONPLUGIN_EXPORTS)
ADD_LIBRARY(InitializationPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(InitializationPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS InitializationPlugin DESTINATION plugins)
#INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
