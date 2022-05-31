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

FIND_PACKAGE(SolveSpace REQUIRED)

SET(PROJECT_HEADERS
    SolveSpaceSolver_Solver.h
    SolveSpaceSolver_Builder.h
    SolveSpaceSolver_Storage.h
    SolveSpaceSolver_EntityWrapper.h
    SolveSpaceSolver_ParameterWrapper.h
    SolveSpaceSolver_ConstraintWrapper.h
    SolveSpaceSolver_ConstraintType.h
)

SET(PROJECT_SOURCES
    SolveSpaceSolver_Solver.cpp
    SolveSpaceSolver_Builder.cpp
    SolveSpaceSolver_Storage.cpp
    SolveSpaceSolver_EntityWrapper.cpp
    SolveSpaceSolver_ParameterWrapper.cpp
    SolveSpaceSolver_ConstraintWrapper.cpp
)

SET(PROJECT_LIBRARIES
    ${SOLVESPACE_LIBRARIES}
    SketchSolver
    ModelAPI
    GeomAPI
)

INCLUDE_DIRECTORIES(
    ${SOLVESPACE_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src/SketchSolver
    ${PROJECT_SOURCE_DIR}/src/SketchPlugin
    ${PROJECT_SOURCE_DIR}/src/ModelAPI
    ${PROJECT_SOURCE_DIR}/src/GeomAPI
    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
)

ADD_LIBRARY(SolveSpaceSolver MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(SolveSpaceSolver ${PROJECT_LIBRARIES})
INSTALL(TARGETS SolveSpaceSolver DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
