## Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
    FiltersPlugin_EdgeSize.h
    FiltersPlugin_FaceSize.h
    FiltersPlugin_VolumeSize.h
    FiltersPlugin_FeatureEdges.h
    FiltersPlugin_ContinuousFaces.h
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
    FiltersPlugin_EdgeSize.cpp
    FiltersPlugin_FaceSize.cpp
    FiltersPlugin_VolumeSize.cpp
    FiltersPlugin_FeatureEdges.cpp
    FiltersPlugin_ContinuousFaces.cpp
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
  filter-EdgeSize.xml
  filter-FaceSize.xml
  filter-VolumeSize.xml
  filter-FeatureEdges.xml
  filter-ContinuousFaces.xml
)

SET(TEXT_RESOURCES
    FiltersPlugin_msg_en.ts
    FiltersPlugin_msg_fr.ts
)

SOURCE_GROUP ("XML Files" FILES ${XML_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

ADD_DEFINITIONS(-DFILTERS_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(FiltersPlugin SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})
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
INSTALL(FILES ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

include(tests.set)

ADD_UNIT_TESTS(${TEST_NAMES})

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/FiltersPlugin")

  install(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)
  install(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})

  set(TMP_TESTS_NAMES)
  foreach(tfile ${TEST_NAMES})
    list(APPEND TMP_TESTS_NAMES "Test/${tfile}")
  endforeach(tfile ${TEST_NAMES})

  install(FILES ${TMP_TESTS_NAMES} DESTINATION ${TEST_INSTALL_DIRECTORY})
endif(${HAVE_SALOME})
