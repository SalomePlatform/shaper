## Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    FeaturesPlugin.h
    FeaturesPlugin_Plugin.h
    FeaturesPlugin_Extrusion.h
    FeaturesPlugin_Revolution.h
    FeaturesPlugin_Rotation.h
    FeaturesPlugin_Translation.h
    FeaturesPlugin_Boolean.h
    FeaturesPlugin_Group.h
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
    FeaturesPlugin_ValidatorTransform.h
    FeaturesPlugin_Validators.h
    FeaturesPlugin_Wire.h
)

SET(PROJECT_SOURCES
    FeaturesPlugin_Plugin.cpp
    FeaturesPlugin_Extrusion.cpp
    FeaturesPlugin_Revolution.cpp
    FeaturesPlugin_Rotation.cpp
    FeaturesPlugin_Translation.cpp
    FeaturesPlugin_Boolean.cpp
    FeaturesPlugin_Group.cpp
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
    FeaturesPlugin_ValidatorTransform.cpp
    FeaturesPlugin_Validators.cpp
    FeaturesPlugin_Wire.cpp
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
  group_widget.xml
  partition_widget.xml
  placement_widget.xml
  intersection_widget.xml
  pipe_widget.xml
  wire_widget.xml
)

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../GeomValidators
  ../Events
)

SET(PROJECT_LIBRARIES
    Events
    ModelAPI
    GeomAPI
    GeomAlgoAPI
    GeomValidators
)

SET(ICON_RESOURCES
  icons/angle.png
  icons/angle_down.png
  icons/angle_up.png
  icons/angle_up_down.png
  icons/angle_up_down_32x32.png
  icons/axis.png
  icons/bool_common.png
  icons/bool_cut.png
  icons/bool_fuse.png
  icons/bool_smash.png
  icons/common.png
  icons/cut.png
  icons/cut_shape.png
  icons/cut_tool.png
  icons/dimension_down.png
  icons/dimension_down_32x32.png
  icons/dimension_up.png
  icons/dimension_up_32x32.png
  icons/dimension_up_down.png
  icons/dimension_up_down_32x32.png
  icons/dimension_v.png
  icons/dimension_v_32x32.png
  icons/dimension_vert_32x32.png
  icons/extrusion.png
  icons/extrusion_cut.png
  icons/extrusion_fuse.png
  icons/extrusionsketch.png
  icons/feature_wire.png
  icons/fusion.png
  icons/intersection.png
  icons/movement.png
  icons/partition.png
  icons/pipe.png
  icons/pipe_binormal_32x32.png
  icons/pipe_locations_32x32.png
  icons/pipe_simple_32x32.png
  icons/placement.png
  icons/placement_from.png
  icons/placement_to.png
  icons/plane.png
  icons/plane_inverted.png
  icons/plane_inverted_32x32.png
  icons/revol_cut.png
  icons/revol_fuse.png
  icons/revolsketch.png
  icons/rotation.png
  icons/shape_group.png
  icons/sketch.png
)

ADD_DEFINITIONS(-DFEATURESPLUGIN_EXPORTS)
ADD_LIBRARY(FeaturesPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(FeaturesPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS FeaturesPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(FILES ${ICON_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Features)

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
               TestMultiBoolean.py
               TestSerialBoolean.py
               TestGroup.py
               TestIntersection.py)
