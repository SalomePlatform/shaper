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
    SketchPlugin_Circle.h
    SketchPlugin_Arc.h
    SketchPlugin_Constraint.h
    SketchPlugin_ConstraintBase.h
    SketchPlugin_ConstraintCoincidence.h
    SketchPlugin_ConstraintDistance.h
    SketchPlugin_ConstraintLength.h
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
    SketchPlugin_Circle.cpp
    SketchPlugin_Arc.cpp
    SketchPlugin_ConstraintBase.cpp
    SketchPlugin_ConstraintCoincidence.cpp
    SketchPlugin_ConstraintDistance.cpp
    SketchPlugin_ConstraintLength.cpp
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
    GeomValidators
    ModelAPI
    SketcherPrs
    GeomDataAPI
    ${CAS_KERNEL}
    ${CAS_SHAPE}
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
  ../GeomValidators
  ../SketcherPrs
)

INSTALL(TARGETS SketchPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)

ADD_UNIT_TESTS(TestSketchPointLine.py
               TestSketchArcCircle.py
               TestConstraintConcidence.py
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
               TestHighload.py
               TestSnowflake.py)
