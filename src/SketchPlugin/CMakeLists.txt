# Copyright (C) 2014-2025  CEA, EDF
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
    SketchPlugin_BSpline.h
    SketchPlugin_BSplineBase.h
    SketchPlugin_BSplinePeriodic.h
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
    SketchPlugin_CurveFitting.h
    SketchPlugin_Ellipse.h
    SketchPlugin_EllipticArc.h
    SketchPlugin_ExternalValidator.h
    SketchPlugin_Feature.h
    SketchPlugin_IntersectionPoint.h
    SketchPlugin_Line.h
    SketchPlugin_MacroArc.h
    SketchPlugin_MacroArcReentrantMessage.h
    SketchPlugin_MacroBSpline.h
    SketchPlugin_MacroCircle.h
    SketchPlugin_MacroEllipse.h
    SketchPlugin_MacroEllipticArc.h
    SketchPlugin_MultiRotation.h
    SketchPlugin_MultiTranslation.h
    SketchPlugin_Offset.h
    SketchPlugin_OverConstraintsResolver.h
    SketchPlugin_Plugin.h
    SketchPlugin_Point.h
    SketchPlugin_Projection.h
    SketchPlugin_Sketch.h
    SketchPlugin_SketchCopy.h
    SketchPlugin_SketchDrawer.h
    SketchPlugin_SketchEntity.h
    SketchPlugin_Split.h
    SketchPlugin_Tools.h
    SketchPlugin_Trim.h
    SketchPlugin_Validators.h
)

SET(PROJECT_SOURCES
    SketchPlugin_Arc.cpp
    SketchPlugin_BSpline.cpp
    SketchPlugin_BSplineBase.cpp
    SketchPlugin_BSplinePeriodic.cpp
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
    SketchPlugin_CurveFitting.cpp
    SketchPlugin_Ellipse.cpp
    SketchPlugin_EllipticArc.cpp
    SketchPlugin_ExternalValidator.cpp
    SketchPlugin_Feature.cpp
    SketchPlugin_IntersectionPoint.cpp
    SketchPlugin_Line.cpp
    SketchPlugin_MacroArc.cpp
    SketchPlugin_MacroBSpline.cpp
    SketchPlugin_MacroCircle.cpp
    SketchPlugin_MacroEllipse.cpp
    SketchPlugin_MacroEllipticArc.cpp
    SketchPlugin_MultiRotation.cpp
    SketchPlugin_MultiTranslation.cpp
    SketchPlugin_Offset.cpp
    SketchPlugin_OverConstraintsResolver.cpp
    SketchPlugin_Plugin.cpp
    SketchPlugin_Point.cpp
    SketchPlugin_Projection.cpp
    SketchPlugin_Sketch.cpp
    SketchPlugin_SketchCopy.cpp
    SketchPlugin_SketchDrawer.cpp
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
    GeomDataAPI
    Locale
    ModelAPI
    ModelGeomAlgo
    ModuleBase
    SketcherPrs
)

SET(XML_RESOURCES
  plugin-Sketch.xml
  plugin-SketchCopy.xml
  plugin-SketchDrawer.xml
)

SET(TEXT_RESOURCES
    SketchPlugin_msg_en.ts
    SketchPlugin_msg_fr.ts
)

SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})
SOURCE_GROUP ("XML Files" FILES ${XML_RESOURCES})

ADD_DEFINITIONS(-DSKETCHPLUGIN_EXPORTS)
ADD_LIBRARY(SketchPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})
TARGET_LINK_LIBRARIES(SketchPlugin ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ../Config
  ../Events
  ../Locale
  ../ModelAPI
  ../ModelGeomAlgo
  ../ModuleBase
  ../GeomAPI
  ../GeomAlgoAPI
  ../GeomDataAPI
  ../SketcherPrs
)

INSTALL(TARGETS SketchPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Sketch)
INSTALL(FILES ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

include(tests.set)

ADD_UNIT_TESTS(${TEST_NAMES})

include(tests.set)

if(${SKETCHER_CHANGE_RADIUS_WHEN_MOVE})
  include(tests_change_radius.set)
  ADD_UNIT_TESTS(${TEST_NAMES_CHANGE_RADIUS})
endif()

ADD_RESTRICTED_TESTS(
  Test20204.py
)

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/SketchPlugin")
  
  install(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)
  if(${SKETCHER_CHANGE_RADIUS_WHEN_MOVE})
    set(TEST_NAMES ${TEST_NAMES} ${TEST_NAMES_CHANGE_RADIUS})
    install(FILES tests_change_radius.set DESTINATION ${TEST_INSTALL_DIRECTORY})
  endif()
  install(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})
  
  set(TMP_TESTS_NAMES)
  foreach(tfile ${TEST_NAMES})
    list(APPEND TMP_TESTS_NAMES "Test/${tfile}")
  endforeach(tfile ${TEST_NAMES})
  
  set(TMP_UNSTABLE_TESTS)
  foreach(tfile ${UNSTABLE_TESTS})
    list(APPEND TMP_UNSTABLE_TESTS "Test/${tfile}")
  endforeach(tfile ${UNSTABLE_TESTS})

  install(FILES ${TMP_UNSTABLE_TESTS} ${TMP_TESTS_NAMES} DESTINATION ${TEST_INSTALL_DIRECTORY})
  install(DIRECTORY data DESTINATION ${TEST_INSTALL_DIRECTORY})
endif(${HAVE_SALOME})
