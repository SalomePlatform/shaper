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
    SketchPlugin
)

INCLUDE_DIRECTORIES(
    ../SketchPlugin
    ../ModelAPI
    ../GeomAPI
)

ADD_DEFINITIONS(-DSKETCHSOLVER_EXPORTS ${BOOST_DEFINITIONS})

ADD_LIBRARY(SketchSolver SHARED 
    ${PROJECT_SOURCES} 
    ${PROJECT_HEADERS}
)

TARGET_LINK_LIBRARIES(SketchSolver
    ${PROJECT_LIBRARIES}
)

INSTALL(TARGETS SketchSolver DESTINATION plugins)
