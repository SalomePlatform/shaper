# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
    FeaturesPlugin.h
    FeaturesPlugin_Plugin.h
    FeaturesPlugin_Extrusion.h
    FeaturesPlugin_Recover.h
    FeaturesPlugin_Revolution.h
    FeaturesPlugin_Rotation.h
    FeaturesPlugin_Translation.h
    FeaturesPlugin_VersionedBoolean.h
    FeaturesPlugin_Boolean.h
    FeaturesPlugin_BooleanCut.h
    FeaturesPlugin_BooleanFuse.h
    FeaturesPlugin_BooleanCommon.h
    FeaturesPlugin_BooleanSmash.h
    FeaturesPlugin_BooleanFill.h
    FeaturesPlugin_Intersection.h
    FeaturesPlugin_Partition.h
    FeaturesPlugin_Pipe.h
    FeaturesPlugin_Loft.h
    FeaturesPlugin_Placement.h
    FeaturesPlugin_PointCloudOnFace.h
    FeaturesPlugin_CompositeBoolean.h
    FeaturesPlugin_CompositeSketch.h
    FeaturesPlugin_ExtrusionBoolean.h
    FeaturesPlugin_ExtrusionCut.h
    FeaturesPlugin_ExtrusionFuse.h
    FeaturesPlugin_RevolutionBoolean.h
    FeaturesPlugin_RevolutionCut.h
    FeaturesPlugin_RevolutionFuse.h
    FeaturesPlugin_Union.h
    FeaturesPlugin_ValidatorTransform.h
    FeaturesPlugin_Validators.h
    FeaturesPlugin_RemoveSubShapes.h
    FeaturesPlugin_Tools.h
    FeaturesPlugin_Symmetry.h
    FeaturesPlugin_Scale.h
    FeaturesPlugin_Sewing.h
    FeaturesPlugin_MultiTranslation.h
    FeaturesPlugin_MultiRotation.h
    FeaturesPlugin_Fillet.h
    FeaturesPlugin_Fillet1D.h
    FeaturesPlugin_Measurement.h
    FeaturesPlugin_FusionFaces.h
    FeaturesPlugin_RemoveResults.h
    FeaturesPlugin_Chamfer.h
    FeaturesPlugin_Copy.h
    FeaturesPlugin_ImportResult.h
    FeaturesPlugin_Defeaturing.h
    FeaturesPlugin_VersionedChFi.h
    FeaturesPlugin_PointCoordinates.h
    FeaturesPlugin_GeometryCalculation.h
    FeaturesPlugin_BoundingBox.h
    FeaturesPlugin_BoundingBoxBase.h
    FeaturesPlugin_InspectBoundingBox.h
    FeaturesPlugin_NormalToFace.h
    FeaturesPlugin_InspectNormalToFace.h
    FeaturesPlugin_GlueFaces.h
)

SET(PROJECT_SOURCES
    FeaturesPlugin_Plugin.cpp
    FeaturesPlugin_Extrusion.cpp
    FeaturesPlugin_Recover.cpp
    FeaturesPlugin_Revolution.cpp
    FeaturesPlugin_Rotation.cpp
    FeaturesPlugin_Translation.cpp
    FeaturesPlugin_VersionedBoolean.cpp
    FeaturesPlugin_Boolean.cpp
    FeaturesPlugin_BooleanCut.cpp
    FeaturesPlugin_BooleanFuse.cpp
    FeaturesPlugin_BooleanCommon.cpp
    FeaturesPlugin_BooleanSmash.cpp
    FeaturesPlugin_BooleanFill.cpp
    FeaturesPlugin_Intersection.cpp
    FeaturesPlugin_Partition.cpp
    FeaturesPlugin_Pipe.cpp
    FeaturesPlugin_Loft.cpp
    FeaturesPlugin_Placement.cpp
    FeaturesPlugin_PointCloudOnFace.cpp
    FeaturesPlugin_CompositeBoolean.cpp
    FeaturesPlugin_CompositeSketch.cpp
    FeaturesPlugin_ExtrusionBoolean.cpp
    FeaturesPlugin_ExtrusionCut.cpp
    FeaturesPlugin_ExtrusionFuse.cpp
    FeaturesPlugin_RevolutionBoolean.cpp
    FeaturesPlugin_RevolutionCut.cpp
    FeaturesPlugin_RevolutionFuse.cpp
    FeaturesPlugin_Union.cpp
    FeaturesPlugin_ValidatorTransform.cpp
    FeaturesPlugin_Validators.cpp
    FeaturesPlugin_RemoveSubShapes.cpp
    FeaturesPlugin_Tools.cpp
    FeaturesPlugin_Symmetry.cpp
    FeaturesPlugin_Scale.cpp
    FeaturesPlugin_Sewing.cpp
    FeaturesPlugin_MultiTranslation.cpp
    FeaturesPlugin_MultiRotation.cpp
    FeaturesPlugin_Fillet.cpp
    FeaturesPlugin_Fillet1D.cpp
    FeaturesPlugin_Measurement.cpp
    FeaturesPlugin_FusionFaces.cpp
    FeaturesPlugin_RemoveResults.cpp
    FeaturesPlugin_Chamfer.cpp
    FeaturesPlugin_Copy.cpp
    FeaturesPlugin_ImportResult.cpp
    FeaturesPlugin_Defeaturing.cpp
    FeaturesPlugin_VersionedChFi.cpp
    FeaturesPlugin_PointCoordinates.cpp
    FeaturesPlugin_GeometryCalculation.cpp
    FeaturesPlugin_BoundingBox.cpp
    FeaturesPlugin_BoundingBoxBase.cpp
    FeaturesPlugin_InspectBoundingBox.cpp
    FeaturesPlugin_NormalToFace.cpp
    FeaturesPlugin_InspectNormalToFace.cpp
    FeaturesPlugin_GlueFaces.cpp
)

SET(XML_RESOURCES
  plugin-Features.xml
  extrusion_widget.xml
  extrusioncut_widget.xml
  extrusionfuse_widget.xml
  revolution_widget.xml
  revolutioncut_widget.xml
  revolutionfuse_widget.xml
  rotation_widget.xml
  translation_widget.xml
  boolean_widget.xml
  boolean_fuse_widget.xml
  boolean_common_widget.xml
  boolean_split_widget.xml
  boolean_smash_widget.xml
  recover_widget.xml
  partition_widget.xml
  placement_widget.xml
  intersection_widget.xml
  pipe_widget.xml
  loft_widget.xml
  remove_subshapes_widget.xml
  union_widget.xml
  symmetry_widget.xml
  scale_widget.xml
  sewing_widget.xml
  multitranslation_widget.xml
  multirotation_widget.xml
  fillet_widget.xml
  fillet1d_widget.xml
  measurement_widget.xml
  fusion_faces_widget.xml
  glue_faces_widget.xml
  chamfer_widget.xml
  copy_widget.xml
  import_result_widget.xml
  defeaturing_widget.xml
  point_coordinates_widget.xml
  geometry_calculation_widget.xml
  bounding_box_widget.xml
  create_bounding_box_widget.xml
  normal_to_face_widget.xml
  create_normal_to_face_widget.xml
)

SET(TEXT_RESOURCES
    FeaturesPlugin_msg_en.ts
    FeaturesPlugin_msg_fr.ts
    FeaturesPlugin_msg_ru.ts
)

SOURCE_GROUP ("XML Files" FILES ${XML_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})


INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../GeomValidators
  ../ModuleBase
  ../Events
  ../Config
  ${OpenCASCADE_INCLUDE_DIR}
)

SET(PROJECT_LIBRARIES
    Events
    ModelAPI
    GeomAPI
    GeomAlgoAPI
    GeomValidators
    Config
    ${OpenCASCADE_Visualization_LIBRARIES}
)

ADD_DEFINITIONS(-DFEATURESPLUGIN_EXPORTS)
ADD_LIBRARY(FeaturesPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})
TARGET_LINK_LIBRARIES(FeaturesPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS FeaturesPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Features)
INSTALL(FILES ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

INCLUDE(tests.set)

ADD_UNIT_TESTS(
  ${TEST_NAMES}
)

IF(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/FeaturesPlugin")

  install(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)
  install(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})

  set(TMP_TESTS_NAMES)
  foreach(tfile ${TEST_NAMES})
    list(APPEND TMP_TESTS_NAMES "Test/${tfile}")
  endforeach(tfile ${TEST_NAMES})

  install(FILES ${TMP_TESTS_NAMES} DESTINATION ${TEST_INSTALL_DIRECTORY})
ENDIF(${HAVE_SALOME})
