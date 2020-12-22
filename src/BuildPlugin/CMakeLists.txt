# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomValidators
                    ${PROJECT_SOURCE_DIR}/src/SketchPlugin
)

SET(PROJECT_HEADERS
    BuildPlugin.h
    BuildPlugin_Plugin.h
    BuildPlugin_Shape.h
    BuildPlugin_Vertex.h
    BuildPlugin_Edge.h
    BuildPlugin_Wire.h
    BuildPlugin_Polyline.h
    BuildPlugin_Interpolation.h
    BuildPlugin_Face.h
    BuildPlugin_Shell.h
    BuildPlugin_Solid.h
    BuildPlugin_CompSolid.h
    BuildPlugin_Compound.h
    BuildPlugin_SubShapes.h
    BuildPlugin_Filling.h
    BuildPlugin_Validators.h
)

SET(PROJECT_SOURCES
    BuildPlugin_Plugin.cpp
    BuildPlugin_Shape.cpp
    BuildPlugin_Vertex.cpp
    BuildPlugin_Edge.cpp
    BuildPlugin_Wire.cpp
    BuildPlugin_Polyline.cpp
    BuildPlugin_Interpolation.cpp
    BuildPlugin_Face.cpp
    BuildPlugin_Shell.cpp
    BuildPlugin_Solid.cpp
    BuildPlugin_CompSolid.cpp
    BuildPlugin_Compound.cpp
    BuildPlugin_SubShapes.cpp
    BuildPlugin_Filling.cpp
    BuildPlugin_Validators.cpp
)

SET(XML_RESOURCES
    plugin-Build.xml
    vertex_widget.xml
    edge_widget.xml
    wire_widget.xml
    polyline_widget.xml
    interpolation_widget.xml
    face_widget.xml
    shell_widget.xml
    solid_widget.xml
    compsolid_widget.xml
    compound_widget.xml
    subshapes_widget.xml
    filling_widget.xml
)

SET(TEXT_RESOURCES
    BuildPlugin_msg_en.ts
    BuildPlugin_msg_fr.ts
)

SOURCE_GROUP ("XML Files" FILES ${XML_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    GeomAPI
    GeomAlgoAPI
    GeomValidators
)

ADD_DEFINITIONS(-DBUILDPLUGIN_EXPORTS)
ADD_LIBRARY(BuildPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})

TARGET_LINK_LIBRARIES(BuildPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS BuildPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Build)

ADD_UNIT_TESTS(TestVertex.py
               TestVertex_ErrorMsg.py
               TestVertex_WholeSketch_1.py
               TestVertex_WholeSketch_2.py
               TestEdge.py
               TestEdge_ByPoints.py
               TestEdge_ErrorMsg.py
               TestEdge_WholeSketch_1.py
               TestEdge_WholeSketch_2.py
               TestEdge_WholeSketch_3.py
               TestEdge_WholeSketch_4.py
               TestWire.py
               TestWire_ErrorMsg.py
               TestWire_WholeSketch_1.py
               TestWire_WholeSketch_2.py
               TestWire_WholeSketch_3.py
               TestWire_WholeSketch_4.py
               TestWire_WholeSketch_5.py
               TestWire_WholeSketch_6.py
               TestPolyline.py
               TestInterpolation.py
               TestFace.py
               TestFace_ErrorMsg.py
               TestFace_WholeSketch_1.py
               TestFace_WholeSketch_2.py
               TestShell.py
               TestShell_WholeSketch_1.py
               TestShell_WholeSketch_2.py
               TestSolid.py
               TestSolid_ErrorMsg.py
               TestCompSolid.py
               TestCompound.py
               TestCompound_ErrorMsg.py
               TestCompound_History.py
               TestSubShapes.py
               TestSubShapes_ErrorMsg.py
               TestFilling.py
               TestFilling_ByEdges.py
               TestFilling_ByWires.py
               TestFilling_Mixed.py
               TestFilling_ErrorMsg.py
               Test1920.py
               Test2398.py
               Test2409.py
               Test2415.py
               Test2439.py
               Test2454.py
               Test3125.py
               Test3271.py
               Test19056.py
               Test20469.py
               Test20513.py
)
