INCLUDE(Common)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
) 

SET(PROJECT_HEADERS
    ParametersPlugin.h
    ParametersPlugin_Plugin.h
    ParametersPlugin_Parameter.h
)
 
SET(PROJECT_SOURCES
    ParametersPlugin_Plugin.cpp
    ParametersPlugin_Parameter.cpp
)

SET(XML_RESOURCES
    plugin-Parameters.xml
)

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
)

ADD_DEFINITIONS(-DPARAMETERSPLUGIN_EXPORTS)
ADD_LIBRARY(ParametersPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(ParametersPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ParametersPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
