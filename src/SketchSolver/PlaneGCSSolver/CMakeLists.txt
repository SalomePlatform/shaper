## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

FIND_PACKAGE(PlaneGCS REQUIRED)

SET(PROJECT_HEADERS
    PlaneGCSSolver_Defs.h
    PlaneGCSSolver_Solver.h
    PlaneGCSSolver_Builder.h
    PlaneGCSSolver_Storage.h
    PlaneGCSSolver_ConstraintWrapper.h
    PlaneGCSSolver_EntityWrapper.h
    PlaneGCSSolver_PointWrapper.h
    PlaneGCSSolver_ScalarWrapper.h
    PlaneGCSSolver_ParameterWrapper.h
    PlaneGCSSolver_AngleWrapper.h
)

SET(PROJECT_SOURCES
    PlaneGCSSolver_Solver.cpp
    PlaneGCSSolver_Builder.cpp
    PlaneGCSSolver_Storage.cpp
    PlaneGCSSolver_ConstraintWrapper.cpp
    PlaneGCSSolver_EntityWrapper.cpp
    PlaneGCSSolver_PointWrapper.cpp
    PlaneGCSSolver_ScalarWrapper.cpp
    PlaneGCSSolver_ParameterWrapper.cpp
    PlaneGCSSolver_AngleWrapper.cpp
)

SET(PROJECT_LIBRARIES
    ${PLANEGCS_LIBRARIES}
    SketchSolver
    ModelAPI
    GeomAPI
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


ADD_LIBRARY(PlaneGCSSolver MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(PlaneGCSSolver ${PROJECT_LIBRARIES})
INSTALL(TARGETS PlaneGCSSolver DESTINATION plugins)
