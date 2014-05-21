INCLUDE(Common)
INCLUDE(FindSolveSpace)

SET(PROJECT_HEADERS
    SketchSolver.h
    SketchSolver_Solver.h
    SketchSolver_ConstraintManager.h
)

SET(PROJECT_SOURCES
    SketchSolver_Solver.cpp
    SketchSolver_ConstraintManager.cpp
)

SET(PROJECT_LIBRARIES
    ${SLVS_LIBRARIES}
    Events
)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src/SketchPlugin
    ${PROJECT_SOURCE_DIR}/src/ModelAPI
    ${PROJECT_SOURCE_DIR}/src/Model
    ${PROJECT_SOURCE_DIR}/src/GeomAPI
    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
    ${PROJECT_SOURCE_DIR}/src/Events
)

ADD_DEFINITIONS(-DSKETCHSOLVER_EXPORTS ${BOOST_DEFINITIONS})

ADD_LIBRARY(SketchSolver MODULE 
    ${PROJECT_SOURCES} 
    ${PROJECT_HEADERS}
)

TARGET_LINK_LIBRARIES(SketchSolver
    ${PROJECT_LIBRARIES}
)

INSTALL(TARGETS SketchSolver DESTINATION plugins)
