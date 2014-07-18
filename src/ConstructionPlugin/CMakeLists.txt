INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    ConstructionPlugin.h
    ConstructionPlugin_Plugin.h
    ConstructionPlugin_Point.h
)

SET(PROJECT_SOURCES
    ConstructionPlugin_Plugin.cpp
    ConstructionPlugin_Point.cpp
)

ADD_DEFINITIONS(-DCONSTRUCTIONPLUGIN_EXPORTS ${BOOST_DEFINITIONS})
ADD_LIBRARY(ConstructionPlugin SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(ConstructionPlugin ${PROJECT_LIBRARIES} ModelAPI GeomAPI GeomAlgoAPI)

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
)

SET(XML_RESOURCES
  plugin-Construction.xml
  point_widget.xml
)

INSTALL(TARGETS ConstructionPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)

ADD_UNIT_TESTS(TestPointName.py
              )
