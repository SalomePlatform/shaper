## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(FindSolveSpace)

SET(PROJECT_HEADERS
    SketchSolver.h
    SketchSolver_Error.h
    SketchSolver_Solver.h
    SketchSolver_Constraint.h
    SketchSolver_ConstraintCoincidence.h
    SketchSolver_ConstraintDistance.h
    SketchSolver_ConstraintEqual.h
    SketchSolver_ConstraintFillet.h
    SketchSolver_ConstraintLength.h
    SketchSolver_ConstraintMirror.h
    SketchSolver_ConstraintRigid.h
    SketchSolver_ConstraintTangent.h
    SketchSolver_Builder.h
    SketchSolver_Group.h
    SketchSolver_ConstraintManager.h
    SketchSolver_Storage.h
    SketchSolver_FeatureStorage.h
)

SET(PROJECT_SOURCES
    SketchSolver_Solver.cpp
    SketchSolver_Constraint.cpp
    SketchSolver_ConstraintCoincidence.cpp
    SketchSolver_ConstraintDistance.cpp
    SketchSolver_ConstraintEqual.cpp
    SketchSolver_ConstraintFillet.cpp
    SketchSolver_ConstraintLength.cpp
    SketchSolver_ConstraintMirror.cpp
    SketchSolver_ConstraintRigid.cpp
    SketchSolver_ConstraintTangent.cpp
    SketchSolver_Builder.cpp
    SketchSolver_Group.cpp
    SketchSolver_ConstraintManager.cpp
    SketchSolver_Storage.cpp
    SketchSolver_FeatureStorage.cpp
)

SET(PROJECT_LIBRARIES
    ${SLVS_LIBRARIES}
    ${CAS_SHAPE}
    Config
    Events
    ModelAPI
    GeomAPI
)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src/Config
    ${PROJECT_SOURCE_DIR}/src/SketchPlugin
    ${PROJECT_SOURCE_DIR}/src/ModelAPI
    ${PROJECT_SOURCE_DIR}/src/GeomAPI
    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
    ${PROJECT_SOURCE_DIR}/src/Events
)

ADD_DEFINITIONS(-DSKETCHSOLVER_EXPORTS)

ADD_LIBRARY(SketchSolver MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS}
)

TARGET_LINK_LIBRARIES(SketchSolver ${PROJECT_LIBRARIES}
)

INSTALL(TARGETS SketchSolver DESTINATION plugins)
