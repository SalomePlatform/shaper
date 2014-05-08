INCLUDE(Common)

SET(PROJECT_HEADERS
    SketchPlugin.h
    SketchPlugin_Feature.h
    SketchPlugin_Plugin.h
    SketchPlugin_Sketch.h
    SketchPlugin_Line.h
    SketchPlugin_Point.h
)

SET(PROJECT_SOURCES
    SketchPlugin_Feature.cpp
    SketchPlugin_Plugin.cpp
    SketchPlugin_Sketch.cpp
    SketchPlugin_Line.cpp
    SketchPlugin_Point.cpp
)

SET(PROJECT_LIBRARIES
    GeomAPI
    GeomAlgoAPI
)

ADD_DEFINITIONS(-DSKETCHPLUGIN_EXPORTS ${BOOST_DEFINITIONS})
ADD_LIBRARY(SketchPlugin SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(SketchPlugin ${PROJECT_LIBRARIES} ModelAPI GeomAPI GeomAlgoAPI)

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../GeomDataAPI
)

SET(XML_RESOURCES
  plugin-Sketch.xml
)

INSTALL(TARGETS SketchPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
