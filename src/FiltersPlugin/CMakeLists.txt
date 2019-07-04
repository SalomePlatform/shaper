## Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
)

SET(PROJECT_LIBRARIES
    ModelAPI
    Events
    Config
    GeomAPI
    GeomAlgoAPI
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
)

ADD_DEFINITIONS(-DFILTERS_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(Filters SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(Filters ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ${OpenCASCADE_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/Config
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
)

INSTALL(TARGETS Filters DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

ADD_UNIT_TESTS(
  TestFilters.py
  TestFilter_BelongsTo.py
  TestFilter_OnPlane.py
  TestFilter_OnLine.py
  TestFilter_OnGeometry_Edge.py
  TestFilter_OnGeometry_Face.py
  TestFilter_OnPlaneSide_Face.py
  TestFilter_OnPlaneSide_Plane.py
  TestFilter_OppositeToEdge.py
  TestFilter_RelativeToSolid_In.py
  TestFilter_RelativeToSolid_Out.py
  TestFilter_RelativeToSolid_On.py
  TestFilter_RelativeToSolid_NotOn.py
  TestFilter_RelativeToSolid_InAndOn.py
  TestFilter_RelativeToSolid_OutAndOn.py
  TestFilter_ExternalFaces.py
  TestFilter_HorizontalFaces.py
  TestFilter_VerticalFaces.py
)
