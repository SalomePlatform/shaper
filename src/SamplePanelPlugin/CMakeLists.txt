## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(UseQtExt)

# additional include directories
INCLUDE_DIRECTORIES(${QT_INCLUDES})

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(PROJECT_HEADERS
    SamplePanelPlugin.h
    SamplePanelPlugin_Feature.h
    SamplePanelPlugin_ModelWidget.h
    SamplePanelPlugin_ModelWidgetCreator.h
    SamplePanelPlugin_Panel.h
    SamplePanelPlugin_Plugin.h
    SamplePanelPlugin_WidgetCreator.h
)

SET(PROJECT_MOC_HEADERS
    SamplePanelPlugin_ModelWidget.h
    SamplePanelPlugin_Panel.h
)

SET(PROJECT_SOURCES
    SamplePanelPlugin_Feature.cpp
    SamplePanelPlugin_ModelWidget.cpp
    SamplePanelPlugin_ModelWidgetCreator.cpp
    SamplePanelPlugin_Panel.cpp
    SamplePanelPlugin_Plugin.cpp
    SamplePanelPlugin_WidgetCreator.cpp
)

SET(PROJECT_LIBRARIES
    Config
    Events
    ModelAPI
    ModuleBase
    ${QT_LIBRARIES}
)

SET(XML_RESOURCES
  plugin-SamplePanel.xml
)
# sources / moc wrappings
QT_WRAP_MOC(PROJECT_AUTOMOC ${PROJECT_MOC_HEADERS})

SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC})

ADD_DEFINITIONS(-DSAMPLEPANELPLUGIN_EXPORTS -DWNT)
ADD_LIBRARY(SamplePanelPlugin MODULE
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
            ${XML_RESOURCES}
            ${PROJECT_AUTOMOC})

TARGET_LINK_LIBRARIES(SamplePanelPlugin ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ../Config
  ../Events
  ../ModelAPI
  ../ModuleBase
)

INSTALL(TARGETS SamplePanelPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
