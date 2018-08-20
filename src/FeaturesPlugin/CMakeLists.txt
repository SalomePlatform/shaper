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
    FeaturesPlugin.h
    FeaturesPlugin_Plugin.h
    FeaturesPlugin_Extrusion.h
    FeaturesPlugin_Recover.h
    FeaturesPlugin_Revolution.h
    FeaturesPlugin_Rotation.h
    FeaturesPlugin_Translation.h
    FeaturesPlugin_Boolean.h
    FeaturesPlugin_BooleanCut.h
    FeaturesPlugin_BooleanFuse.h
    FeaturesPlugin_BooleanCommon.h
    FeaturesPlugin_BooleanSmash.h
    FeaturesPlugin_BooleanFill.h
    FeaturesPlugin_Intersection.h
    FeaturesPlugin_Partition.h
    FeaturesPlugin_Pipe.h
    FeaturesPlugin_Placement.h
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
    FeaturesPlugin_MultiTranslation.h
    FeaturesPlugin_MultiRotation.h
    FeaturesPlugin_Fillet.h
    FeaturesPlugin_Measurement.h
)

SET(PROJECT_SOURCES
    FeaturesPlugin_Plugin.cpp
    FeaturesPlugin_Extrusion.cpp
    FeaturesPlugin_Recover.cpp
    FeaturesPlugin_Revolution.cpp
    FeaturesPlugin_Rotation.cpp
    FeaturesPlugin_Translation.cpp
    FeaturesPlugin_Boolean.cpp
    FeaturesPlugin_BooleanCut.cpp
    FeaturesPlugin_BooleanFuse.cpp
    FeaturesPlugin_BooleanCommon.cpp
    FeaturesPlugin_BooleanSmash.cpp
    FeaturesPlugin_Intersection.cpp
    FeaturesPlugin_Partition.cpp
    FeaturesPlugin_Pipe.cpp
    FeaturesPlugin_Placement.cpp
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
    FeaturesPlugin_MultiTranslation.cpp
    FeaturesPlugin_MultiRotation.cpp
    FeaturesPlugin_Fillet.cpp
    FeaturesPlugin_Measurement.cpp
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
  boolean_smash_widget.xml
  recover_widget.xml
  partition_widget.xml
  placement_widget.xml
  intersection_widget.xml
  pipe_widget.xml
  remove_subshapes_widget.xml
  union_widget.xml
  symmetry_widget.xml
  scale_widget.xml
  multitranslation_widget.xml
  multirotation_widget.xml
  fillet_widget.xml
  measurement_widget.xml
)

SET(TEXT_RESOURCES
    FeaturesPlugin_msg_en.ts
    FeaturesPlugin_msg_ru.ts
)

SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})


INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../GeomValidators
  ../Events
  ../Config
  ${CAS_INCLUDE_DIRS}
)

SET(PROJECT_LIBRARIES
    Events
    ModelAPI
    GeomAPI
    GeomAlgoAPI
    GeomValidators
    Config
    ${CAS_VIEWER}
    ${CAS_SHAPE}
)

ADD_DEFINITIONS(-DFEATURESPLUGIN_EXPORTS)
ADD_LIBRARY(FeaturesPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})
TARGET_LINK_LIBRARIES(FeaturesPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS FeaturesPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Features)
INSTALL(FILES ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

ADD_UNIT_TESTS(TestExtrusion.py
               TestExtrusionCut.py
               TestExtrusionFuse.py
               TestRevolution.py
               TestRevolutionCut.py
               TestRevolutionFuse.py
               TestCompositeFeaturesOnCompSolids.py
               TestPartition.py
               TestPlacement.py
               TestTranslation.py
               TestRotation.py
               TestBoolean.py
               TestBooleanCompSolids.py
               TestBooleanSmash.py
               TestBooleanFill.py
               TestBooleanFillWithPlane.py
               TestMultiBoolean.py
               TestSerialBoolean.py
               TestIntersection.py
               TestUnion.py
               TestUnionFaces.py
               TestRemoveSubShapes.py
               TestRemoveSubShapes2.py
               TestPipe.py
               TestRecover.py
               TestRecover1798.py
               TestFillEdgeVertex.py
               TestFillEdgeEdge.py
               TestFillEdgeEdgeIntersected.py
               TestFillEdgeWire.py
               TestFillEdgeFace.py
               TestFillEdgeShell.py
               TestFillEdgeSolid.py
               TestFillEdgeSolidIntersected.py
               TestFillWireVertex.py
               TestFillWireEdge.py
               TestFillWireWire.py
               TestFillWireFace.py
               TestFillWireShell.py
               TestFillWireSolid.py
               TestFillFaceVertex.py
               TestFillFaceEdge.py
               TestFillFaceEdgeInside.py
               TestFillFaceEdgePerpendicular.py
               TestFillFaceWire.py
               TestFillFaceFace.py
               TestFillFaceShell.py
               TestFillFaceSolid.py
               TestFillCompFaceSolid.py
               TestFillShellVertex.py
               TestFillShellEdge.py
               TestFillShellConstrPlane.py
               TestFillShellWire.py
               TestFillShellFace.py
               TestFillShellShell.py
               TestFillShellSolid.py
               TestFillSolid2ConstructionPlanes.py
               TestFillSolidEdge.py
               TestFillSolidFace.py
               TestFillSolidCompFace.py
               TestFillSolidShell.py
               TestFillSolidCompShell.py
               TestPartition2Faces.py
               TestPartition2Solids.py
               TestPartition2Wires.py
               TestPartitionBox4Planes.py
               TestPartitionEdgeSolid.py
               TestPartitionFace2Solid.py
#               TestPartitionFaceSolid.py
               TestPartitionFaceWire.py
               TestPartitionInclinedFaceSolid.py
               TestPartitionWireFaceSolid.py
               TestUnion4CurvedFaces.py
               TestUnion4Faces.py
               TestUnionOfUnion.py
               TestMeasurementLength.py
               TestMeasurementDistance.py
               TestMeasurementRadius.py
               TestMeasurementAngle.py
               Test1379.py
               Test1922.py
               Test1942.py
               Test1915.py
               Test2023.py
               Test2046.py
               Test2038.py
               Test2172.py
               Test2194.py
               Test2197_1.py
               Test2197_2.py
               Test2197_3.py
               Test2197_4.py
               Test2215.py
               Test2222.py
               Test2233.py
               Test2231.py
               Test2240.py
               Test2246.py
               Test2248.py
               Test2251.py
               Test2255.py
               Test2289.py
               Test2304.py
               Test2304_2.py
               Test2375.py
               Test2377.py
               Test2394.py
               Test2395.py
               Test2419_1.py
               Test2419_2.py
               Test2419_3.py
               Test2465.py
               Test2495.py
               Test2514.py
               Test2520.py
               TestBooleanCut_CompSolid_CompSolid.py
               TestBooleanCut_CompSolidCompound_CompSolidCompound.py
               TestBooleanCut_Edge_Edge.py
               TestBooleanCut_Edge_Face.py
               TestBooleanCut_EdgeCompound_EdgeCompound.py
               TestBooleanCut_EdgeCompound_Solid.py
               TestBooleanCut_Face_Face.py
               TestBooleanCut_Face_Solid.py
               TestBooleanCut_FaceCompound_FaceCompound.py
               TestBooleanCut_Shell_Shell.py
               TestBooleanCut_ShellCompound_ShellCompound.py
               TestBooleanCut_Solid_Solid.py
               TestBooleanCut_SolidCompound_SolidCompound.py
               TestBooleanCut_Vertex_Vertex.py
               TestBooleanCut_VertexCompound_Solid.py
               TestBooleanCut_VertexCompound_VertexCompound.py
               TestBooleanCut_Wire_Face.py
               TestBooleanCut_Wire_Wire.py
               TestBooleanCut_WireCompound_WireCompound.py
               TestBooleanSmash_Face_Face.py
               TestBooleanFuse_SimpleMode.py
               TestBooleanFuse_RemoveEdges.py
               TestBooleanCommon_Vertex_Vertex.py
               TestBooleanCommon_VertexCompound_VertexCompound.py
               TestBooleanCommon_Edge_Edge.py
               TestBooleanCommon_EdgeCompound_EdgeCompound.py
               TestBooleanCommon_Wire_Wire.py
               TestBooleanCommon_WireCompound_WireCompound.py
               TestBooleanCommon_Face_Face.py
               TestBooleanCommon_FaceCompound_FaceCompound.py
               TestBooleanCommon_Shell_Shell.py
               TestBooleanCommon_ShellCompound_ShellCompound.py
               TestBooleanCommon_Solid_Face.py
               TestBooleanCommon_Solid_Shell.py
               TestBooleanCommon_Solid_Solid.py
               TestBooleanCommon_SolidCompound_Face.py
               TestBooleanCommon_SolidCompound_Shell.py
               TestBooleanCommon_SolidCompound_SolidCompound.py
               TestBooleanCommon_CompSolid_Face.py
               TestBooleanCommon_CompSolid_Shell.py
               TestBooleanCommon_CompSolid_CompSolid.py
               TestBooleanCommon_CompSolidCompound_Face.py
               TestBooleanCommon_CompSolidCompound_Shell.py
               TestBooleanCommon_CompSolidCompound_CompSolidCompound.py
)
