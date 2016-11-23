INCLUDE(Common)
INCLUDE(FindPython)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
                    ${PROJECT_SOURCE_DIR}/src/ParametersPlugin
                    ${SUIT_INCLUDE}
                    ${PYTHON_INCLUDE_DIR}
)

SET(PROJECT_HEADERS
    InitializationPlugin.h
    InitializationPlugin_Plugin.h
    InitializationPlugin_EvalListener.h
    InitializationPlugin_PyInterp.h 
)

SET(PROJECT_SOURCES
    InitializationPlugin_Plugin.cpp
    InitializationPlugin_EvalListener.cpp
    InitializationPlugin_PyInterp.cpp
)

MESSAGE(STATUS "PYTHON_LIBRARIES (ParametersPlugin): ${PYTHON_LIBRARIES}")

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    ${PyInterp}
    ${PYTHON_LIBRARIES}
)

ADD_DEFINITIONS(-DINITIALIZATIONPLUGIN_EXPORTS -DHAVE_DEBUG_PYTHON)
ADD_LIBRARY(InitializationPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(InitializationPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS InitializationPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
#INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)

