## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
