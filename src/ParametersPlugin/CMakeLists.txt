INCLUDE(Common)
INCLUDE(FindPython)
INCLUDE(UnitTest)
SET(CMAKE_AUTOMOC ON)

SET(PROJECT_HEADERS
    ParametersPlugin.h
    ParametersPlugin_Plugin.h
    ParametersPlugin_Parameter.h
    ParametersPlugin_PyInterp.h
    ParametersPlugin_Validators.h
    ParametersPlugin_EvalListener.h
    ParametersPlugin_WidgetCreator.h
    ParametersPlugin_ParametersMgr.h
    ParametersPlugin_WidgetParamsMgr.h
)

SET(PROJECT_SOURCES
    ParametersPlugin_Plugin.cpp
    ParametersPlugin_Parameter.cpp
    ParametersPlugin_PyInterp.cpp
    ParametersPlugin_Validators.cpp
    ParametersPlugin_EvalListener.cpp
    ParametersPlugin_WidgetCreator.cpp
    ParametersPlugin_ParametersMgr.cpp
    ParametersPlugin_WidgetParamsMgr.cpp
)

SET(XML_RESOURCES
    plugin-Parameters.xml
)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
                    ${PROJECT_SOURCE_DIR}/src/ModuleBase
                    ${SUIT_INCLUDE}
                    ${PYTHON_INCLUDE_DIR}
)

MESSAGE(STATUS "PYTHON_LIBRARIES (ParametersPlugin): ${PYTHON_LIBRARIES}")

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    ModuleBase
    ${PyInterp}
    ${PYTHON_LIBRARIES}
    ${QT_LIBRARIES}
)

ADD_DEFINITIONS(-DPARAMETERSPLUGIN_EXPORTS -DHAVE_DEBUG_PYTHON)
ADD_LIBRARY(ParametersPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(ParametersPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ParametersPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

ADD_UNIT_TESTS(TestParameterCreation.py
               TestParameterRename.py
               TestParameterChangeValue.py
              )
