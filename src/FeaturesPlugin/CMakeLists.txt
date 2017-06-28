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
)

SET(PROJECT_SOURCES
    FeaturesPlugin_Plugin.cpp
    FeaturesPlugin_Extrusion.cpp
    FeaturesPlugin_Recover.cpp
    FeaturesPlugin_Revolution.cpp
    FeaturesPlugin_Rotation.cpp
    FeaturesPlugin_Translation.cpp
    FeaturesPlugin_Boolean.cpp
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
)

SET(PROJECT_LIBRARIES
    Events
    ModelAPI
    GeomAPI
    GeomAlgoAPI
    GeomValidators
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
               TestMultiBoolean.py
               TestSerialBoolean.py
               TestIntersection.py
               TestUnion.py
               TestRemoveSubShapes.py
               TestPipe.py
               TestRecover.py
               TestRecover1798.py
               Test1922.py
               Test1942.py
               Test1915.py
               Test2023.py
               Test2046.py
               Test2038.py
               Test2194.py
)
