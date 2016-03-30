## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    SamplePanelPlugin.h
    SamplePanelPlugin_Feature.h
    SamplePanelPlugin_Panel.h
    SamplePanelPlugin_Plugin.h
    SamplePanelPlugin_WidgetCreator.h
)

SET(PROJECT_SOURCES
    SamplePanelPlugin_Feature.cpp
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

ADD_DEFINITIONS(-DSAMPLEPANELPLUGIN_EXPORTS -DWNT)
ADD_LIBRARY(SamplePanelPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(SamplePanelPlugin ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ../Config
  ../Events
  ../ModelAPI
  ../ModuleBase
)

INSTALL(TARGETS SamplePanelPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
