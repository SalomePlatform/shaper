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
    FeaturesPlugin_Partition.h
    FeaturesPlugin_Placement.h
    FeaturesPlugin_CompositeBoolean.h
    FeaturesPlugin_CompositeSketch.h
    FeaturesPlugin_ExtrusionBoolean.h
    FeaturesPlugin_ExtrusionSketch.h
    FeaturesPlugin_ExtrusionCut.h
    FeaturesPlugin_ExtrusionFuse.h
    FeaturesPlugin_RevolutionSketch.h
    FeaturesPlugin_RevolutionBoolean.h
    FeaturesPlugin_RevolutionCut.h
    FeaturesPlugin_RevolutionFuse.h
    FeaturesPlugin_ValidatorTransform.h
)

SET(PROJECT_SOURCES
    FeaturesPlugin_Plugin.cpp
    FeaturesPlugin_Extrusion.cpp
    FeaturesPlugin_Revolution.cpp
    FeaturesPlugin_Rotation.cpp
    FeaturesPlugin_Translation.cpp
    FeaturesPlugin_Boolean.cpp
    FeaturesPlugin_Group.cpp
    FeaturesPlugin_Partition.cpp
    FeaturesPlugin_Placement.cpp
    FeaturesPlugin_CompositeBoolean.cpp
    FeaturesPlugin_CompositeSketch.cpp
    FeaturesPlugin_ExtrusionSketch.cpp
    FeaturesPlugin_ExtrusionBoolean.cpp
    FeaturesPlugin_ExtrusionCut.cpp
    FeaturesPlugin_ExtrusionFuse.cpp
    FeaturesPlugin_RevolutionSketch.cpp
    FeaturesPlugin_RevolutionBoolean.cpp
    FeaturesPlugin_RevolutionCut.cpp
    FeaturesPlugin_RevolutionFuse.cpp
    FeaturesPlugin_ValidatorTransform.cpp
)

SET(XML_RESOURCES
  plugin-Features.xml
  extrusion_widget.xml
  extrusionsketch_widget.xml
  extrusioncut_widget.xml
  extrusionfuse_widget.xml
  revolution_widget.xml
  revolutionsketch_widget.xml
  revolutioncut_widget.xml
  revolutionfuse_widget.xml
  rotation_widget.xml
  translation_widget.xml
  boolean_widget.xml
  group_widget.xml
  partition_widget.xml
  placement_widget.xml
)

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../Events
)

SET(PROJECT_LIBRARIES
    Events
    ModelAPI 
    GeomAPI 
    GeomAlgoAPI
)

ADD_DEFINITIONS(-DFEATURESPLUGIN_EXPORTS)
ADD_LIBRARY(FeaturesPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(FeaturesPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS FeaturesPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)

ADD_UNIT_TESTS(TestExtrusion.py
               TestExtrusionSketch.py
               TestExtrusionCut.py
               TestExtrusionFuse.py
               TestRevolution.py
               TestRevolutionSketch.py
               TestRevolutionCut.py
               TestRevolutionFuse.py
               TestCompositeFeaturesOnCompSolids.py
               TestPartition.py
               TestPlacement.py
               TestTranslation.py
               TestRotation.py
               TestBoolean.py
               TestBooleanCompSolids.py
               TestMultiBoolean.py
               TestSerialBoolean.py
               TestGroup.py)
