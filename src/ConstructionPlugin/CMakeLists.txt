INCLUDE(Common)

SET(PROJECT_HEADERS
    ConstructionPlugin.h
    ConstructionPlugin_Plugin.h
    ConstructionPlugin_Point.h
)

SET(PROJECT_SOURCES
    ConstructionPlugin_Plugin.cxx
    ConstructionPlugin_Point.cxx
)

ADD_DEFINITIONS(-DCONSTRUCTIONPLUGIN_EXPORTS ${BOOST_DEFINITIONS})
ADD_LIBRARY(ConstructionPlugin SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(ConstructionPlugin ${PROJECT_LIBRARIES} ModelAPI)

INCLUDE_DIRECTORIES(
  ../ModelAPI
)

SET(XML_RESOURCES
  plugin-Construction.xml
)

INSTALL(TARGETS ConstructionPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
