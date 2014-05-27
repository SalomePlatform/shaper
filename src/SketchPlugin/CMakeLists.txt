INCLUDE(Common)

SET(PROJECT_HEADERS
    SketchPlugin.h
    SketchPlugin_Feature.h
    SketchPlugin_Plugin.h
    SketchPlugin_Sketch.h
    SketchPlugin_Line.h
    SketchPlugin_Point.h
    SketchPlugin_Circle.h
    SketchPlugin_Arc.h
    SketchPlugin_Constraint.h
    SketchPlugin_ConstraintCoincidence.h
    SketchPlugin_ConstraintDistance.h
    SketchPlugin_ConstraintDiameter.h
    SketchPlugin_ConstraintParallel.h
    SketchPlugin_ConstraintPerpendicular.h
)

SET(PROJECT_SOURCES
    SketchPlugin_Feature.cpp
    SketchPlugin_Plugin.cpp
    SketchPlugin_Sketch.cpp
    SketchPlugin_Line.cpp
    SketchPlugin_Point.cpp
    SketchPlugin_Circle.cpp
    SketchPlugin_Arc.cpp
    SketchPlugin_ConstraintCoincidence.cpp
    SketchPlugin_ConstraintDistance.cpp
    SketchPlugin_ConstraintDiameter.cpp
    SketchPlugin_ConstraintParallel.cpp
    SketchPlugin_ConstraintPerpendicular.cpp
)

SET(PROJECT_LIBRARIES
    GeomAPI
    GeomAlgoAPI
    ModelAPI
)

ADD_DEFINITIONS(-DSKETCHPLUGIN_EXPORTS ${BOOST_DEFINITIONS})
ADD_LIBRARY(SketchPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(SketchPlugin ${PROJECT_LIBRARIES})

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
