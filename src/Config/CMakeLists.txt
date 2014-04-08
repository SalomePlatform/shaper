CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

INCLUDE(Common)
INCLUDE(XMLProcessing)

INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/Event) 

SET(PROJECT_HEADERS
  Config.h
  Config_FeatureMessage.h
  Config_XMLReader.h
  Config_ModuleReader.h
  Config_FeatureReader.h
  Config_WidgetAPI.h
  Config_WidgetReader.h
  Config_PointerMessage.h
 )
 
SET(PROJECT_SOURCES
  Config_FeatureMessage.cpp
  Config_XMLReader.cpp
  Config_ModuleReader.cpp
  Config_FeatureReader.cpp
  Config_WidgetAPI.cpp
  Config_WidgetReader.cpp
  Config_PointerMessage.cpp
)

SET(XML_RESOURCES
  plugin-PartSet.xml
  plugins.xml
)

SET(PROJECT_LIBRARIES
    Event
    ${LIBXML2_LIBRARIES}
)

SOURCE_GROUP ("Resource Files" FILES ${XML_RESOURCES})

ADD_DEFINITIONS(-DCONFIG_EXPORTS)
ADD_LIBRARY(Config SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(Config ${PROJECT_LIBRARIES})

INSTALL(TARGETS Config DESTINATION bin)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
