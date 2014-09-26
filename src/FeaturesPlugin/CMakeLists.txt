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

SET(XML_RESOURCES
  plugin-Features.xml
  extrusion_widget.xml
  boolean_widget.xml
)

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
)

SET(PROJECT_LIBRARIES
    ModelAPI 
    GeomAPI 
    GeomAlgoAPI
    ${CAS_KERNEL}
    ${CAS_SHAPE}
)

ADD_DEFINITIONS(-DFEATURESPLUGIN_EXPORTS ${BOOST_DEFINITIONS})
ADD_LIBRARY(FeaturesPlugin SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(FeaturesPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS FeaturesPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
