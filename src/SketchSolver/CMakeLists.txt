## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)

SET(PROJECT_HEADERS
    SketchSolver.h
    SketchSolver_Error.h
    SketchSolver_Constraint.h
    SketchSolver_ConstraintAngle.h
    SketchSolver_ConstraintCoincidence.h
    SketchSolver_ConstraintDistance.h
    SketchSolver_ConstraintEqual.h
    SketchSolver_ConstraintLength.h
    SketchSolver_ConstraintMirror.h
    SketchSolver_ConstraintFixed.h
    SketchSolver_ConstraintTangent.h
    SketchSolver_ConstraintMulti.h
    SketchSolver_ConstraintMultiRotation.h
    SketchSolver_ConstraintMultiTranslation.h
    SketchSolver_ConstraintMovement.h
    SketchSolver_Group.h
    SketchSolver_Builder.h
    SketchSolver_IConstraintWrapper.h
    SketchSolver_IEntityWrapper.h
    SketchSolver_IParameterWrapper.h
    SketchSolver_ISolver.h
    SketchSolver_Manager.h
    SketchSolver_Storage.h
)

SET(PROJECT_SOURCES
    SketchSolver_Constraint.cpp
    SketchSolver_ConstraintAngle.cpp
    SketchSolver_ConstraintCoincidence.cpp
    SketchSolver_ConstraintDistance.cpp
    SketchSolver_ConstraintEqual.cpp
    SketchSolver_ConstraintLength.cpp
    SketchSolver_ConstraintMirror.cpp
    SketchSolver_ConstraintFixed.cpp
    SketchSolver_ConstraintTangent.cpp
    SketchSolver_ConstraintMulti.cpp
    SketchSolver_ConstraintMultiRotation.cpp
    SketchSolver_ConstraintMultiTranslation.cpp
    SketchSolver_ConstraintMovement.cpp
    SketchSolver_Group.cpp
    SketchSolver_Builder.cpp
    SketchSolver_Manager.cpp
    SketchSolver_Storage.cpp
)

SET(PROJECT_LIBRARIES
    Config
    Events
    ModelAPI
    GeomAPI
    GeomDataAPI
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

ADD_LIBRARY(SketchSolver SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS}
)

TARGET_LINK_LIBRARIES(SketchSolver ${PROJECT_LIBRARIES}
)

INSTALL(TARGETS SketchSolver DESTINATION plugins)


# Include specific solvers
FIND_FILE(SSPath "SolveSpaceSolver/CMakeLists.txt" PATHS "${CMAKE_CURRENT_SOURCE_DIR}")
STRING(COMPARE NOTEQUAL ${SSPath} "SSPath-NOTFOUND" SSPath_FOUND)
FIND_FILE(GCSPath "PlainGCSSolver/CMakeLists.txt" PATHS "${CMAKE_CURRENT_SOURCE_DIR}")
STRING(COMPARE NOTEQUAL ${GCSPath} "GCSPath-NOTFOUND" GCSPath_FOUND)

IF(${SSPath_FOUND} OR ${GCSPath_FOUND})
  IF(${SSPath_FOUND})
    MESSAGE(STATUS "SolveSpaceSolver plugin found in ${SSPath}")
    ADD_SUBDIRECTORY(SolveSpaceSolver)
  ENDIF()
  IF(${GCSPath_FOUND})
    MESSAGE(STATUS "PlainGCSSolver plugin found in ${GCSPath}")
    ADD_SUBDIRECTORY(PlainGCSSolver)
  ENDIF()
ELSE()
  MESSAGE(WARNING "No sketch solver plugin is found")
ENDIF()
