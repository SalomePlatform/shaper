## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)

SET(PROJECT_HEADERS
    PartSetPlugin.h
    PartSetPlugin_Plugin.h
    PartSetPlugin_Part.h
    PartSetPlugin_Duplicate.h
    PartSetPlugin_Remove.h
)

SET(PROJECT_SOURCES
    PartSetPlugin_Plugin.cpp
    PartSetPlugin_Part.cpp
    PartSetPlugin_Duplicate.cpp
    PartSetPlugin_Remove.cpp
)

SET(XML_RESOURCES
  plugin-PartSet.xml
)

ADD_DEFINITIONS(-DPARTSETPLUGIN_EXPORTS)
ADD_LIBRARY(PartSetPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(PartSetPlugin ${PROJECT_LIBRARIES} ModelAPI)

INCLUDE_DIRECTORIES(
  ../Events
  ../ModelAPI
  ../GeomAPI
)

INSTALL(TARGETS PartSetPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/PartSet)
