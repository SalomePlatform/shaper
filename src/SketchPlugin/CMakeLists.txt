## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    SketchPlugin.h
    SketchPlugin_Arc.h
    SketchPlugin_Circle.h
    SketchPlugin_Constraint.h
    SketchPlugin_ConstraintAngle.h
    SketchPlugin_ConstraintBase.h
    SketchPlugin_ConstraintCoincidence.h
    SketchPlugin_ConstraintCollinear.h
    SketchPlugin_ConstraintDistance.h
    SketchPlugin_ConstraintEqual.h
    SketchPlugin_Fillet.h
    SketchPlugin_ConstraintHorizontal.h
    SketchPlugin_ConstraintLength.h
    SketchPlugin_ConstraintMiddle.h
    SketchPlugin_ConstraintMirror.h
    SketchPlugin_ConstraintParallel.h
    SketchPlugin_ConstraintPerpendicular.h
    SketchPlugin_ConstraintRadius.h
    SketchPlugin_ConstraintRigid.h
    SketchPlugin_ConstraintTangent.h
    SketchPlugin_ConstraintVertical.h
    SketchPlugin_ExternalValidator.h
    SketchPlugin_Feature.h
    SketchPlugin_IntersectionPoint.h
    SketchPlugin_Line.h
    SketchPlugin_MacroArc.h
    SketchPlugin_MacroArcReentrantMessage.h
    SketchPlugin_MacroCircle.h
    SketchPlugin_MultiRotation.h
    SketchPlugin_MultiTranslation.h
    SketchPlugin_Plugin.h
    SketchPlugin_Point.h
    SketchPlugin_Projection.h
    SketchPlugin_Sketch.h
    SketchPlugin_SketchEntity.h
    SketchPlugin_Split.h
    SketchPlugin_Tools.h
    SketchPlugin_Trim.h
    SketchPlugin_Validators.h
)

SET(PROJECT_SOURCES
    SketchPlugin_Arc.cpp
    SketchPlugin_Circle.cpp
    SketchPlugin_Constraint.cpp
    SketchPlugin_ConstraintAngle.cpp
    SketchPlugin_ConstraintBase.cpp
    SketchPlugin_ConstraintCoincidence.cpp
    SketchPlugin_ConstraintCollinear.cpp
    SketchPlugin_ConstraintDistance.cpp
    SketchPlugin_ConstraintEqual.cpp
    SketchPlugin_Fillet.cpp
    SketchPlugin_ConstraintHorizontal.cpp
    SketchPlugin_ConstraintLength.cpp
    SketchPlugin_ConstraintMiddle.cpp
    SketchPlugin_ConstraintMirror.cpp
    SketchPlugin_ConstraintParallel.cpp
    SketchPlugin_ConstraintPerpendicular.cpp
    SketchPlugin_ConstraintRadius.cpp
    SketchPlugin_ConstraintRigid.cpp
    SketchPlugin_ConstraintTangent.cpp
    SketchPlugin_ConstraintVertical.cpp
    SketchPlugin_ExternalValidator.cpp
    SketchPlugin_Feature.cpp
    SketchPlugin_IntersectionPoint.cpp
    SketchPlugin_Line.cpp
    SketchPlugin_MacroArc.cpp
    SketchPlugin_MacroCircle.cpp
    SketchPlugin_MultiRotation.cpp
    SketchPlugin_MultiTranslation.cpp
    SketchPlugin_Plugin.cpp
    SketchPlugin_Point.cpp
    SketchPlugin_Projection.cpp
    SketchPlugin_Sketch.cpp
    SketchPlugin_SketchEntity.cpp
    SketchPlugin_Split.cpp
    SketchPlugin_Tools.cpp
    SketchPlugin_Trim.cpp
    SketchPlugin_Validators.cpp
)

SET(PROJECT_LIBRARIES
    Config
    GeomAPI
    GeomAlgoAPI
    ModelAPI
    ModelGeomAlgo
    SketcherPrs
    GeomDataAPI
)

SET(XML_RESOURCES
  plugin-Sketch.xml
)

SET(TEXT_RESOURCES
	SketchPlugin_msg_en.ts
)

SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

ADD_DEFINITIONS(-DSKETCHPLUGIN_EXPORTS)
ADD_LIBRARY(SketchPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})
TARGET_LINK_LIBRARIES(SketchPlugin ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ../Config
  ../Events
  ../ModelAPI
  ../ModelGeomAlgo
  ../GeomAPI
  ../GeomAlgoAPI
  ../GeomDataAPI
  ../SketcherPrs
)

INSTALL(TARGETS SketchPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Sketch)
INSTALL(FILES ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

ADD_UNIT_TESTS(TestSketchPointLine.py
               TestCreateArcByCenterStartEnd.py
               TestCreateArcByThreePoints.py
               TestCreateArcByTangentEdge.py
               TestCreateArcChangeType.py
               TestCreateCircleByCenterAndPassed.py
               TestCreateCircleByThreePoints.py
               TestCreateCircleChangeType.py
               TestConstraintCoincidence.py
               TestConstraintCollinear.py
               TestConstraintLength.py
               TestConstraintDistance.py
               TestConstraintParallel.py
               TestConstraintPerpendicular.py
               TestConstraintRadius.py
               TestConstraintFixed.py
               TestConstraintHorizontal.py
               TestConstraintVertical.py
               TestConstraintEqual.py
               TestConstraintTangent.py
               TestConstraintAngle.py
               TestConstraintMiddlePoint.py
               TestMirror.py
               TestMultiRotation.py
               TestMultiTranslation.py
               TestFillet.py
               TestFilletInteracting.py
               TestRectangle.py
               TestProjection.py
               TestSplit.py
               TestHighload.py
               TestSnowflake.py
               TestArcBehavior.py
               TestRemoveSketch.py
               Test1061.py
               Test1673.py
               Test1924.py
               Test1966.py
               Test1967.py
               Test2095.py
               Test2115.py
               TestTrimArc01.py
               TestTrimArc02.py
               TestTrimArc03.py
               TestTrimArc04.py
               TestTrimArc05.py
               TestTrimArc06.py
               TestTrimArc07.py
               TestTrimCircle01.py
               TestTrimCircle02.py
               TestTrimCircle03.py
               TestTrimCircle04.py
               TestTrimCircle05.py
               TestTrimCircleAndArc01.py
               TestTrimLine01.py
               TestTrimLine02.py
               Test2239.py
)
