## Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
##
## See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
##

INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    FiltersPlugin.h
    FiltersPlugin_Plugin.h
    FiltersPlugin_Selection.h
    FiltersPlugin_HorizontalFace.h
    FiltersPlugin_VerticalFace.h
    FiltersPlugin_BelongsTo.h
    FiltersPlugin_OnPlane.h
    FiltersPlugin_OnLine.h
    FiltersPlugin_OnGeometry.h
    FiltersPlugin_OnPlaneSide.h
    FiltersPlugin_OppositeToEdge.h
    FiltersPlugin_RelativeToSolid.h
    FiltersPlugin_ExternalFaces.h
    FiltersPlugin_Validators.h
)

SET(PROJECT_SOURCES
    FiltersPlugin_Plugin.cpp
    FiltersPlugin_Selection.cpp
    FiltersPlugin_HorizontalFace.cpp
    FiltersPlugin_VerticalFace.cpp
    FiltersPlugin_BelongsTo.cpp
    FiltersPlugin_OnPlane.cpp
    FiltersPlugin_OnLine.cpp
    FiltersPlugin_OnGeometry.cpp
    FiltersPlugin_OnPlaneSide.cpp
    FiltersPlugin_OppositeToEdge.cpp
    FiltersPlugin_RelativeToSolid.cpp
    FiltersPlugin_ExternalFaces.cpp
    FiltersPlugin_Validators.cpp
)

SET(PROJECT_LIBRARIES
    ModelAPI
    Events
    Config
    GeomAPI
    GeomAlgoAPI
    GeomValidators
)

SET(PROJECT_PYFILES
    FiltersPlugin_TopoConnectedFaces.py
)

SET(XML_RESOURCES
  plugin-Filters.xml
  filter-BelongsTo.xml
  filter-OnGeometry.xml
  filter-OnLine.xml
  filter-OnPlane.xml
  filter-OnPlaneSide.xml
  filter-OppositeToEdge.xml
  filter-RelativeToSolid.xml
  filter-TopoConnectedFaces.xml
)

SOURCE_GROUP ("XML Files" FILES ${XML_RESOURCES})

ADD_DEFINITIONS(-DFILTERS_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(FiltersPlugin SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(FiltersPlugin ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ${OpenCASCADE_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/Config
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
  ${PROJECT_SOURCE_DIR}/src/GeomValidators
  ${PROJECT_SOURCE_DIR}/src/CollectionPlugin
)

INSTALL(TARGETS FiltersPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(FILES ${PROJECT_PYFILES} DESTINATION ${SHAPER_INSTALL_ADDONS})


ADD_UNIT_TESTS(
  TestFilters.py
  TestFilters_Mixed1.py
  TestFilters_Mixed2.py
  TestFilters_Supported.py
  TestFilters_Validator.py
  TestFilters_Xml.py
  TestFilters_FilterName.py
  TestFilters_IsReversed.py
  TestFilters_Remove.py
  TestFilter_BelongsTo.py
  TestFilter_BelongsTo_Exclude.py
  TestFilter_OnPlane.py
  TestFilter_OnPlane_Exclude.py
  TestFilter_OnLine.py
  TestFilter_OnLine_Exclude.py
  TestFilter_OnGeometry_Edge1.py
  TestFilter_OnGeometry_Edge2.py
  TestFilter_OnGeometry_Face1.py
  TestFilter_OnGeometry_Face2.py
  TestFilter_OnGeometry_Face3.py
  TestFilter_OnGeometry_Face4.py
  TestFilter_OnGeometry_Exclude_Edge1.py
  TestFilter_OnGeometry_Exclude_Edge2.py
  TestFilter_OnGeometry_Exclude_Face1.py
  TestFilter_OnGeometry_Exclude_Face2.py
  TestFilter_OnGeometry_Exclude_Face3.py
  TestFilter_OnGeometry_Exclude_Face4.py
  TestFilter_OnPlaneSide_Face.py
  TestFilter_OnPlaneSide_Plane.py
  TestFilter_OnPlaneSide_Exclude_Face.py
  TestFilter_OnPlaneSide_Exclude_Plane.py
  TestFilter_OppositeToEdge.py
  TestFilter_OppositeToEdge_Exclude.py
  TestFilter_RelativeToSolid_In.py
  TestFilter_RelativeToSolid_Out.py
  TestFilter_RelativeToSolid_On.py
  TestFilter_RelativeToSolid_NotOn.py
  TestFilter_RelativeToSolid_InAndOn.py
  TestFilter_RelativeToSolid_OutAndOn.py
  TestFilter_RelativeToSolid_Exclude_In.py
  TestFilter_RelativeToSolid_Exclude_Out.py
  TestFilter_RelativeToSolid_Exclude_On.py
  TestFilter_RelativeToSolid_Exclude_NotOn.py
  TestFilter_RelativeToSolid_Exclude_InAndOn.py
  TestFilter_RelativeToSolid_Exclude_OutAndOn.py
  TestFilter_ExternalFaces1.py
  TestFilter_ExternalFaces2.py
  TestFilter_ExternalFaces_Exclude1.py
  TestFilter_ExternalFaces_Exclude2.py
  TestFilter_HorizontalFaces.py
  TestFilter_HorizontalFaces_Exclude.py
  TestFilter_VerticalFaces.py
  TestFilter_VerticalFaces_Exclude.py
  TestFilter_TopoConnectedFaces_Vertex1.py
  TestFilter_TopoConnectedFaces_Vertex2.py
  TestFilter_TopoConnectedFaces_Vertex3.py
  TestFilter_TopoConnectedFaces_Prop_Vertex1.py
  TestFilter_TopoConnectedFaces_Prop_Vertex2.py
  TestFilter_TopoConnectedFaces_Prop_Vertex3.py
  TestFilter_TopoConnectedFaces_Edge1.py
  TestFilter_TopoConnectedFaces_Edge2.py
  TestFilter_TopoConnectedFaces_Edge3.py
  TestFilter_TopoConnectedFaces_Prop_Edge1.py
  TestFilter_TopoConnectedFaces_Prop_Edge2.py
  TestFilter_TopoConnectedFaces_Prop_Edge3.py
  TestFilter_TopoConnectedFaces_Face1.py
  TestFilter_TopoConnectedFaces_Face2.py
  TestFilter_TopoConnectedFaces_Face3.py
  TestFilter_TopoConnectedFaces_Prop_Face1.py
  TestFilter_TopoConnectedFaces_Prop_Face2.py
  TestFilter_TopoConnectedFaces_Prop_Face3.py
  TestFilter_TopoConnectedFaces_Exclude_Vertex1.py
  TestFilter_TopoConnectedFaces_Exclude_Vertex2.py
  TestFilter_TopoConnectedFaces_Exclude_Vertex3.py
  TestFilter_TopoConnectedFaces_Prop_Exclude_Vertex1.py
  TestFilter_TopoConnectedFaces_Prop_Exclude_Vertex2.py
  TestFilter_TopoConnectedFaces_Prop_Exclude_Vertex3.py
  TestFilter_TopoConnectedFaces_Exclude_Edge1.py
  TestFilter_TopoConnectedFaces_Exclude_Edge2.py
  TestFilter_TopoConnectedFaces_Exclude_Edge3.py
  TestFilter_TopoConnectedFaces_Prop_Exclude_Edge1.py
  TestFilter_TopoConnectedFaces_Prop_Exclude_Edge2.py
  TestFilter_TopoConnectedFaces_Prop_Exclude_Edge3.py
  TestFilter_TopoConnectedFaces_Exclude_Face1.py
  TestFilter_TopoConnectedFaces_Exclude_Face2.py
  TestFilter_TopoConnectedFaces_Exclude_Face3.py
  TestFilter_TopoConnectedFaces_Prop_Exclude_Face1.py
  TestFilter_TopoConnectedFaces_Prop_Exclude_Face2.py
  TestFilter_TopoConnectedFaces_Prop_Exclude_Face3.py
  Test2946.py
  Test2951.py
  Test17924.py
  Test17962.py
  TestFilter_OnLine_Multi.py
  TestFilter_OnPlane_Multi.py
  TestFilter_OnPlaneSide_Multi.py
  TestFilter_RelativeToSolid_Multi.py
  TestFilter_TopoConnectedFaces_Multi.py
)
