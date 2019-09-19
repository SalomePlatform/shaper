# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

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
    SketchPlugin_ConstraintCoincidenceInternal.h
    SketchPlugin_ConstraintCollinear.h
    SketchPlugin_ConstraintDistance.h
    SketchPlugin_ConstraintDistanceAlongDir.h
    SketchPlugin_ConstraintDistanceHorizontal.h
    SketchPlugin_ConstraintDistanceVertical.h
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
    SketchPlugin_Ellipse.h
    SketchPlugin_ExternalValidator.h
    SketchPlugin_Feature.h
    SketchPlugin_IntersectionPoint.h
    SketchPlugin_Line.h
    SketchPlugin_MacroArc.h
    SketchPlugin_MacroArcReentrantMessage.h
    SketchPlugin_MacroCircle.h
    SketchPlugin_MacroEllipse.h
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
    SketchPlugin_SketchDrawer.h
)

SET(PROJECT_SOURCES
    SketchPlugin_Arc.cpp
    SketchPlugin_Circle.cpp
    SketchPlugin_Constraint.cpp
    SketchPlugin_ConstraintAngle.cpp
    SketchPlugin_ConstraintCoincidence.cpp
    SketchPlugin_ConstraintCoincidenceInternal.cpp
    SketchPlugin_ConstraintCollinear.cpp
    SketchPlugin_ConstraintDistance.cpp
    SketchPlugin_ConstraintDistanceAlongDir.cpp
    SketchPlugin_ConstraintDistanceHorizontal.cpp
    SketchPlugin_ConstraintDistanceVertical.cpp
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
    SketchPlugin_Ellipse.cpp
    SketchPlugin_ExternalValidator.cpp
    SketchPlugin_Feature.cpp
    SketchPlugin_IntersectionPoint.cpp
    SketchPlugin_Line.cpp
    SketchPlugin_MacroArc.cpp
    SketchPlugin_MacroCircle.cpp
    SketchPlugin_MacroEllipse.cpp
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
    SketchPlugin_SketchDrawer.cpp
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
  plugin-SketchDrawer.xml
)

SET(TEXT_RESOURCES
	SketchPlugin_msg_en.ts
	SketchPlugin_msg_fr.ts
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

ADD_UNIT_TESTS(
  Test1061.py
  Test1577.py
  Test1578.py
  Test1673.py
  Test1779.py
  Test1924.py
  Test1963.py
  Test1966.py
  Test1967.py
  Test2034_1.py
  Test2034_2.py
  Test2034_3.py
  Test2062.py
  Test2095.py
  Test2115.py
  Test2119.py
  Test2134.py
  Test2157.py
  Test2157_2.py
  Test2224.py
  Test2229.py
  Test2239.py
  Test2273.py
  Test2280.py
  Test2287.py
  Test2341.py
  Test2376.py
  Test2390.py
  Test2393.py
  Test2425.py
  Test2427.py
  Test2440.py
  Test2481.py
  Test2654.py
  Test2711.py
  Test2741.py
  Test2810.py
  Test2824.py
  Test2860.py
  Test2894.py
  TestArcBehavior.py
  TestChangeSketchPlane1.py
  TestChangeSketchPlane2.py
  TestChangeSketchPlane3.py
  TestConstraintAngle.py
  TestConstraintCoincidence.py
  TestConstraintCollinear.py
  TestConstraintDistance.py
  TestConstraintDistanceBehavior.py
  TestConstraintDistanceHorizontal.py
  TestConstraintDistanceVertical.py
  TestConstraintEqual.py
  TestConstraintFixed.py
  TestConstraintHorizontal.py
  TestConstraintHorizontalValidator.py
  TestConstraintLength.py
  TestConstraintMiddlePoint.py
  TestConstraintMiddlePointOnArc.py
  TestConstraintParallel.py
  TestConstraintPerpendicular.py
  TestConstraintPerpendicularArcLine.py
  TestConstraintRadius.py
  TestConstraintRadiusFailure.py
  TestConstraintTangent.py
  TestConstraintVertical.py
  TestCreateArcByCenterStartEnd.py
  TestCreateArcByTangentEdge.py
  TestCreateArcByThreePoints.py
  TestCreateArcByTransversalLine.py
  TestCreateArcChangeType.py
  TestCreateCircleByCenterAndPassed.py
  TestCreateCircleByThreePoints.py
  TestCreateCircleChangeType.py
  TestCreateEllipseByCenterSemiaxisAndPassed.py
  TestCreateEllipseByMajorAxisAndPassed.py
  TestCreateEllipseByExternal.py
  TestDegeneratedGeometry.py
  TestDistanceDump.py
  TestDistanceSignedVsUnsigned01.py
  TestDistanceSignedVsUnsigned02.py
  TestDistanceSignedVsUnsigned03.py
  TestDistanceSignedVsUnsigned04.py
  TestDistanceSignedVsUnsigned05.py
  TestEdgesOrder.py
  TestFillet.py
  TestFilletAfterFolder.py
  TestFilletInteracting.py
  TestHighload.py
  TestIntersectionChangeEdge.py
  TestIntersectionWithCircle.py
  TestIntersectionWithLine.py
  TestIntersectionWithSpline.py
  TestMirror.py
  TestMultiRotation.py
  TestMultiRotation01.py
  TestMultiRotation02.py
  TestMultiRotation03.py
  TestMultiRotation04.py
  TestMultiRotation05.py
  TestMultiRotationWithParameter.py
  TestMultiTranslation.py
  TestPresentation.py
  TestProjection.py
  TestProjectionIntoResult.py
  TestProjectionUpdate.py
  TestRectangle.py
  TestRemoveEllipse.py
  TestRemoveSketch.py
  TestSignedDistancePointLine.py
  TestSignedDistancePointPoint.py
  TestSketchDrawer.py
  TestSketchPointLine.py
  TestSnowflake.py
  TestSplit.py
  TestSplitLine.py
  TestSplitPreview.py
  TestTrimArc01.py
  TestTrimArc02.py
  TestTrimArc03.py
  TestTrimArc04.py
  TestTrimArc05.py
  TestTrimArc06.py
  TestTrimArc07.py
  TestTrimArc08.py
  TestTrimCircle01.py
  TestTrimCircle02.py
  TestTrimCircle03.py
  TestTrimCircle04.py
  TestTrimCircle05.py
  TestTrimCircleAndArc01.py
  TestTrimLine01.py
  TestTrimLine02.py
  TestTrimLine03.py
  TestTrimPreview.py
  TestUpdateSketch.py
)

if(${SKETCHER_CHANGE_RADIUS_WHEN_MOVE})
  ADD_UNIT_TESTS(
    TestMoveArc.py
    TestMoveCircle.py
    TestMoveEllipse.py
    TestMoveLine.py
    TestMovementComplex.py
    TestMovePoint.py
  )
endif()
