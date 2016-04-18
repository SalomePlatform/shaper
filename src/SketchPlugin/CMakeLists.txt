## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    SketchPlugin.h
    SketchPlugin_Feature.h
    SketchPlugin_Plugin.h
    SketchPlugin_Sketch.h
    SketchPlugin_SketchEntity.h
    SketchPlugin_Line.h
    SketchPlugin_Point.h
    SketchPlugin_IntersectionPoint.h
    SketchPlugin_Circle.h
    SketchPlugin_Arc.h
    SketchPlugin_Constraint.h
    SketchPlugin_ConstraintBase.h
    SketchPlugin_ConstraintCoincidence.h
    SketchPlugin_ConstraintCollinear.h
    SketchPlugin_ConstraintDistance.h
    SketchPlugin_ConstraintLength.h
    SketchPlugin_ConstraintMiddle.h
    SketchPlugin_ConstraintParallel.h
    SketchPlugin_ConstraintPerpendicular.h
    SketchPlugin_ConstraintRadius.h
    SketchPlugin_ConstraintRigid.h
    SketchPlugin_ConstraintHorizontal.h
    SketchPlugin_ConstraintVertical.h
    SketchPlugin_ConstraintEqual.h
    SketchPlugin_ConstraintTangent.h
    SketchPlugin_ConstraintMirror.h
    SketchPlugin_ConstraintFillet.h
    SketchPlugin_ConstraintAngle.h
    SketchPlugin_MultiRotation.h
    SketchPlugin_MultiTranslation.h
    SketchPlugin_ExternalValidator.h
    SketchPlugin_Validators.h
    SketchPlugin_Tools.h
)

SET(PROJECT_SOURCES
    SketchPlugin_Feature.cpp
    SketchPlugin_Plugin.cpp
    SketchPlugin_Sketch.cpp
    SketchPlugin_SketchEntity.cpp
    SketchPlugin_Line.cpp
    SketchPlugin_Point.cpp
    SketchPlugin_IntersectionPoint.cpp
    SketchPlugin_Circle.cpp
    SketchPlugin_Arc.cpp
    SketchPlugin_Constraint.cpp
    SketchPlugin_ConstraintBase.cpp
    SketchPlugin_ConstraintCoincidence.cpp
    SketchPlugin_ConstraintCollinear.cpp
    SketchPlugin_ConstraintDistance.cpp
    SketchPlugin_ConstraintLength.cpp
    SketchPlugin_ConstraintMiddle.cpp
    SketchPlugin_ConstraintParallel.cpp
    SketchPlugin_ConstraintPerpendicular.cpp
    SketchPlugin_ConstraintRadius.cpp
    SketchPlugin_ConstraintRigid.cpp
    SketchPlugin_ConstraintHorizontal.cpp
    SketchPlugin_ConstraintVertical.cpp
    SketchPlugin_ConstraintEqual.cpp
    SketchPlugin_ConstraintTangent.cpp
    SketchPlugin_ConstraintMirror.cpp
    SketchPlugin_ConstraintFillet.cpp
    SketchPlugin_ConstraintAngle.cpp
    SketchPlugin_MultiRotation.cpp
    SketchPlugin_MultiTranslation.cpp
    SketchPlugin_ExternalValidator.cpp
    SketchPlugin_Validators.cpp
    SketchPlugin_Tools.cpp
)

SET(PROJECT_LIBRARIES
    Config
    GeomAPI
    GeomAlgoAPI
    ModelAPI
    SketcherPrs
    GeomDataAPI
)

SET(XML_RESOURCES
  plugin-Sketch.xml
)

ADD_DEFINITIONS(-DSKETCHPLUGIN_EXPORTS)
ADD_LIBRARY(SketchPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(SketchPlugin ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ../Config
  ../Events
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../GeomDataAPI
  ../SketcherPrs
)

INSTALL(TARGETS SketchPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Sketch)

ADD_UNIT_TESTS(TestSketchPointLine.py
               TestSketchArcCircle.py
               TestConstraintConcidence.py
               # TestConstraintCollinear.py
               TestConstraintLength.py
               TestConstraintDistance.py
               TestConstraintParallel.py
               TestConstraintPerpendicular.py
               TestConstraintRadius.py
               TestConstraintRigid.py
               TestConstraintHorizontal.py
               TestConstraintVertical.py
               TestConstraintEqual.py
               TestConstraintTangent.py
               TestConstraintMirror.py
               #TestConstraintAngle.py
               TestConstraintMiddlePoint.py
               TestMultiRotation.py
               TestMultiTranslation.py
               TestFillet.py
               TestRectangle.py
               TestHighload.py
               TestSnowflake.py)
