INCLUDE(Common)

SET(PROJECT_HEADERS
    FeaturesPlugin.h
    FeaturesPlugin_Plugin.h
    FeaturesPlugin_Extrusion.h
	FeaturesPlugin_Boolean.h
)

SET(PROJECT_SOURCES
    FeaturesPlugin_Plugin.cpp
    FeaturesPlugin_Extrusion.cpp
	FeaturesPlugin_Boolean.cpp
)

ADD_DEFINITIONS(-DFEATURESPLUGIN_EXPORTS ${BOOST_DEFINITIONS})
ADD_LIBRARY(FeaturesPlugin SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(FeaturesPlugin ${PROJECT_LIBRARIES} ModelAPI GeomAPI GeomAlgoAPI)

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
)

SET(XML_RESOURCES
  plugin-Features.xml
  extrusion_widget.xml
  boolean_widget.xml
)

INSTALL(TARGETS FeaturesPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
