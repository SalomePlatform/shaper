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

FIND_PACKAGE(PlaneGCS REQUIRED)

SET(PLANEGCSSOLVER_HEADERS
    PlaneGCSSolver_Defs.h
    PlaneGCSSolver_Solver.h
    PlaneGCSSolver_Storage.h
    PlaneGCSSolver_ConstraintWrapper.h
    PlaneGCSSolver_EdgeWrapper.h
    PlaneGCSSolver_EntityWrapper.h
    PlaneGCSSolver_PointWrapper.h
    PlaneGCSSolver_PointArrayWrapper.h
    PlaneGCSSolver_ScalarWrapper.h
    PlaneGCSSolver_ScalarArrayWrapper.h
    PlaneGCSSolver_AngleWrapper.h
    PlaneGCSSolver_BooleanWrapper.h
    PlaneGCSSolver_Tools.h
    PlaneGCSSolver_GeoExtensions.h
)

SET(PLANEGCSSOLVER_SOURCES
    PlaneGCSSolver_Solver.cpp
    PlaneGCSSolver_Storage.cpp
    PlaneGCSSolver_ConstraintWrapper.cpp
    PlaneGCSSolver_EdgeWrapper.cpp
    PlaneGCSSolver_PointWrapper.cpp
    PlaneGCSSolver_PointArrayWrapper.cpp
    PlaneGCSSolver_ScalarWrapper.cpp
    PlaneGCSSolver_ScalarArrayWrapper.cpp
    PlaneGCSSolver_AngleWrapper.cpp
    PlaneGCSSolver_BooleanWrapper.cpp
    PlaneGCSSolver_Tools.cpp
    PlaneGCSSolver_GeoExtensions.cpp
)

SET(PLANEGCSSOLVER_BUILDER_HEADERS
    PlaneGCSSolver_EntityBuilder.h
    PlaneGCSSolver_AttributeBuilder.h
    PlaneGCSSolver_FeatureBuilder.h
    PlaneGCSSolver_EntityDestroyer.h
)

SET(PLANEGCSSOLVER_BUILDER_SOURCES
    PlaneGCSSolver_AttributeBuilder.cpp
    PlaneGCSSolver_FeatureBuilder.cpp
    PlaneGCSSolver_EntityDestroyer.cpp
)

SET(PLANEGCSSOLVER_UPDATER_HEADERS
    PlaneGCSSolver_Update.h
    PlaneGCSSolver_UpdateFeature.h
    PlaneGCSSolver_UpdateCoincidence.h
)

SET(PLANEGCSSOLVER_UPDATER_SOURCES
    PlaneGCSSolver_UpdateFeature.cpp
    PlaneGCSSolver_UpdateCoincidence.cpp
)

SET(PROJECT_LIBRARIES
    ${PLANEGCS_LIBRARIES}
    ${SKETCHSOLVER_LIBRARIES}
)

INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    ${PLANEGCS_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src/SketchSolver
    ${PROJECT_SOURCE_DIR}/src/SketchPlugin
    ${PROJECT_SOURCE_DIR}/src/ModelAPI
    ${PROJECT_SOURCE_DIR}/src/GeomAPI
    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
)

SOURCE_GROUP ("Resource Files" FILES ${SKETCHSOLVER_TEXT_RESOURCES})
SOURCE_GROUP ("Header Files\\Builders" FILES ${PLANEGCSSOLVER_BUILDER_HEADERS})
SOURCE_GROUP ("Source Files\\Builders" FILES ${PLANEGCSSOLVER_BUILDER_SOURCES})
SOURCE_GROUP ("Header Files\\Updaters" FILES ${PLANEGCSSOLVER_UPDATER_HEADERS})
SOURCE_GROUP ("Source Files\\Updaters" FILES ${PLANEGCSSOLVER_UPDATER_SOURCES})
SOURCE_GROUP ("Header Files\\SketchSolver" FILES ${SKETCHSOLVER_HEADERS})
SOURCE_GROUP ("Source Files\\SketchSolver" FILES ${SKETCHSOLVER_SOURCES})
SOURCE_GROUP ("Header Files\\Constraints" FILES ${SKETCHSOLVER_CONSTRAINT_HEADERS})
SOURCE_GROUP ("Source Files\\Constraints" FILES ${SKETCHSOLVER_CONSTRAINT_SOURCES})
SOURCE_GROUP ("Header Files\\PlaneGCS" FILES ${PLANEGCSSOLVER_HEADERS})
SOURCE_GROUP ("Source Files\\PlaneGCS" FILES ${PLANEGCSSOLVER_SOURCES})

SET(PROJECT_HEADERS
    ${PLANEGCSSOLVER_HEADERS}
    ${PLANEGCSSOLVER_BUILDER_HEADERS}
    ${PLANEGCSSOLVER_UPDATER_HEADERS}
    ${SKETCHSOLVER_HEADERS}
    ${SKETCHSOLVER_CONSTRAINT_HEADERS}
)

SET(PROJECT_SOURCES
    ${PLANEGCSSOLVER_SOURCES}
    ${PLANEGCSSOLVER_BUILDER_SOURCES}
    ${PLANEGCSSOLVER_UPDATER_SOURCES}
    ${SKETCHSOLVER_SOURCES}
    ${SKETCHSOLVER_CONSTRAINT_SOURCES}
)

if(${SKETCHER_CHANGE_RADIUS_WHEN_MOVE})
  ADD_DEFINITIONS(-DCHANGE_RADIUS_WHILE_MOVE)
endif()

##ADD_DEFINITIONS(-Wno-deprecated -Wignored-attributes -Wmisleading-indentation -Wreorder)
ADD_DEFINITIONS(-w)

ADD_LIBRARY(PlaneGCSSolver MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${SKETCHSOLVER_TEXT_RESOURCES})
TARGET_LINK_LIBRARIES(PlaneGCSSolver ${PROJECT_LIBRARIES} ${SKETCHSOLVER_LIBRARIES})
INSTALL(TARGETS PlaneGCSSolver DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${SKETCHSOLVER_TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
