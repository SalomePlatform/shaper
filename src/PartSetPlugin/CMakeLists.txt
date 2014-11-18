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

ADD_DEFINITIONS(-DPARTSETPLUGIN_EXPORTS)
ADD_LIBRARY(PartSetPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(PartSetPlugin ${PROJECT_LIBRARIES} ModelAPI)

INCLUDE_DIRECTORIES(
  ../ModelAPI
)

SET(XML_RESOURCES
  plugin-PartSet.xml
)

INSTALL(TARGETS PartSetPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
