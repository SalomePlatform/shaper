INCLUDE(Common)
INCLUDE(FindPython)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    ParametersPlugin.h
    ParametersPlugin_Plugin.h
    ParametersPlugin_Parameter.h
    ParametersPlugin_PyInterp.h
    ParametersPlugin_Validators.h
)
 
SET(PROJECT_SOURCES
    ParametersPlugin_Plugin.cpp
    ParametersPlugin_Parameter.cpp
    ParametersPlugin_PyInterp.cpp
    ParametersPlugin_Validators.cpp
)

SET(XML_RESOURCES
    plugin-Parameters.xml
)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${SUIT_INCLUDE}
                    ${PYTHON_INCLUDE_DIR}
) 

MESSAGE(STATUS "PYTHON_LIBRARIES (ParametersPlugin): ${PYTHON_LIBRARIES}")

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    ${PyInterp}
    ${PYTHON_LIBRARIES}
)

ADD_DEFINITIONS(-DPARAMETERSPLUGIN_EXPORTS -DHAVE_DEBUG_PYTHON)
ADD_LIBRARY(ParametersPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(ParametersPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ParametersPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)

ADD_UNIT_TESTS(TestParameterCreation.py
              )
