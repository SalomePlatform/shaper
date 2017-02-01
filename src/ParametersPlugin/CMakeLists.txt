INCLUDE(Common)
INCLUDE(UnitTest)
INCLUDE(UseQtExt)

# additional include directories
INCLUDE_DIRECTORIES(${QT_INCLUDES})

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(PROJECT_HEADERS
    ParametersPlugin.h
    ParametersPlugin_Plugin.h
    ParametersPlugin_Parameter.h
    ParametersPlugin_EvalListener.h 
    ParametersPlugin_Validators.h
    ParametersPlugin_WidgetCreator.h
    ParametersPlugin_ParametersMgr.h
    ParametersPlugin_WidgetParamsMgr.h
)

SET(PROJECT_MOC_HEADERS
    ParametersPlugin_WidgetParamsMgr.h
)

SET(PROJECT_SOURCES
    ParametersPlugin_Plugin.cpp
    ParametersPlugin_Parameter.cpp
   ParametersPlugin_EvalListener.cpp
    ParametersPlugin_Validators.cpp
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
                    ${CAS_INCLUDE_DIRS}
)

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    ModuleBase
    ${QT_LIBRARIES}
)

ADD_DEFINITIONS(-DPARAMETERSPLUGIN_EXPORTS ${CAS_DEFINITIONS})

SET(TEXT_RESOURCES
	ParametersPlugin_msg_ru.ts
	ParametersPlugin_msg_en.ts
)

# QT4_CREATE_TRANSLATION(QM_RESOURCES
# 					   ${PROJECT_SOURCES}
# 					   ${TEXT_RESOURCES}
# 					   OPTIONS -extensions cpp -no-recursive
# 					   )

#SET(PROJECT_AUTOMOC
#    ${CMAKE_CURRENT_BINARY_DIR}/ParametersPlugin_automoc.cpp
#)
# sources / moc wrappings
QT_WRAP_MOC(PROJECT_AUTOMOC ${PROJECT_MOC_HEADERS})


SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${QM_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

ADD_LIBRARY(ParametersPlugin MODULE
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
            ${XML_RESOURCES}
            ${TEXT_RESOURCES}
            ${QM_RESOURCES}
            ${PROJECT_AUTOMOC}
)


TARGET_LINK_LIBRARIES(ParametersPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ParametersPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(FILES ${QM_RESOURCES} DESTINATION ${SHAPER_INSTALL_QM_RESOURCES})

ADD_UNIT_TESTS(TestParameterCreation.py
               TestParameterRename.py
               TestParameterChangeValue.py
               Test1806.py 
              )
