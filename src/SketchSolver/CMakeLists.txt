## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

# Avoid using SolveSpace
SET(NO_SolveSpace TRUE)

# Convert files in list to full name
FUNCTION(TO_FULL_PATH theFILES)
  SET(locFiles "")
  FOREACH(locFile ${${theFILES}})
    SET(locFiles ${locFiles} ${CMAKE_CURRENT_SOURCE_DIR}/${locFile})
  ENDFOREACH()
  SET(${theFILES} ${locFiles} PARENT_SCOPE)
ENDFUNCTION(TO_FULL_PATH)


INCLUDE(Common)

SET(SKETCHSOLVER_HEADERS
    SketchSolver.h
    SketchSolver_Error.h
    SketchSolver_Group.h
    SketchSolver_Builder.h
    SketchSolver_IConstraintWrapper.h
    SketchSolver_IEntityWrapper.h
    SketchSolver_ISolver.h
    SketchSolver_Manager.h
    SketchSolver_Storage.h
)

SET(SKETCHSOLVER_CONSTRAINT_HEADERS
    SketchSolver_Constraint.h
    SketchSolver_ConstraintAngle.h
    SketchSolver_ConstraintCoincidence.h
    SketchSolver_ConstraintCollinear.h
    SketchSolver_ConstraintDistance.h
    SketchSolver_ConstraintEqual.h
    SketchSolver_ConstraintLength.h
    SketchSolver_ConstraintMiddle.h
    SketchSolver_ConstraintMirror.h
    SketchSolver_ConstraintFixed.h
    SketchSolver_ConstraintTangent.h
    SketchSolver_ConstraintMulti.h
    SketchSolver_ConstraintMultiRotation.h
    SketchSolver_ConstraintMultiTranslation.h
)

SET(SKETCHSOLVER_SOURCES
    SketchSolver_Group.cpp
    SketchSolver_Builder.cpp
    SketchSolver_Manager.cpp
    SketchSolver_Storage.cpp
)

SET(SKETCHSOLVER_CONSTRAINT_SOURCES
    SketchSolver_Constraint.cpp
    SketchSolver_ConstraintAngle.cpp
    SketchSolver_ConstraintCoincidence.cpp
    SketchSolver_ConstraintCollinear.cpp
    SketchSolver_ConstraintDistance.cpp
    SketchSolver_ConstraintEqual.cpp
    SketchSolver_ConstraintLength.cpp
    SketchSolver_ConstraintMiddle.cpp
    SketchSolver_ConstraintMirror.cpp
    SketchSolver_ConstraintFixed.cpp
    SketchSolver_ConstraintTangent.cpp
    SketchSolver_ConstraintMulti.cpp
    SketchSolver_ConstraintMultiRotation.cpp
    SketchSolver_ConstraintMultiTranslation.cpp
)

SET(SKETCHSOLVER_LIBRARIES
    Config
    Events
    ModelAPI
    GeomAPI
    GeomDataAPI
)

SET(SKETCHSOLVER_TEXT_RESOURCES
    SketchSolver_msg_en.ts
)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src/Config
    ${PROJECT_SOURCE_DIR}/src/SketchPlugin
    ${PROJECT_SOURCE_DIR}/src/ModelAPI
    ${PROJECT_SOURCE_DIR}/src/GeomAPI
    ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
    ${PROJECT_SOURCE_DIR}/src/Events
)

#SOURCE_GROUP ("Resource Files" FILES ${SKETCHSOLVER_TEXT_RESOURCES})
#
#ADD_DEFINITIONS(-DSKETCHSOLVER_EXPORTS)
#
#ADD_LIBRARY(SketchSolver SHARED ${SKETCHSOLVER_SOURCES} ${SKETCHSOLVER_CONSTRAINT_SOURCES} ${SKETCHSOLVER_HEADERS} ${SKETCHSOLVER_CONSTRAINT_HEADERS} ${SKETCHSOLVER_TEXT_RESOURCES}
#)
#
#TARGET_LINK_LIBRARIES(SketchSolver ${SKETCHSOLVER_LIBRARIES}
#)
#
#INSTALL(TARGETS SketchSolver DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
#INSTALL(FILES ${SKETCHSOLVER_TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

# Set full path to source files
TO_FULL_PATH(SKETCHSOLVER_SOURCES)
TO_FULL_PATH(SKETCHSOLVER_CONSTRAINT_SOURCES)
TO_FULL_PATH(SKETCHSOLVER_HEADERS)
TO_FULL_PATH(SKETCHSOLVER_CONSTRAINT_HEADERS)
TO_FULL_PATH(SKETCHSOLVER_TEXT_RESOURCES)


# Include specific solvers
IF(NO_SolveSpace)
  SET(SSPath_FOUND FALSE)
ELSE()
  FIND_FILE(SSPath "SolveSpaceSolver/CMakeLists.txt" PATHS "${CMAKE_CURRENT_SOURCE_DIR}")
  STRING(COMPARE NOTEQUAL ${SSPath} "SSPath-NOTFOUND" SSPath_FOUND)
ENDIF()

IF(NO_PlaneGCS)
  SET(GCSPath_FOUND FALSE)
ELSE()
  FIND_FILE(GCSPath "PlaneGCSSolver/CMakeLists.txt" PATHS "${CMAKE_CURRENT_SOURCE_DIR}")
  STRING(COMPARE NOTEQUAL ${GCSPath} "GCSPath-NOTFOUND" GCSPath_FOUND)
ENDIF()

IF(${SSPath_FOUND} OR ${GCSPath_FOUND})
  IF(${SSPath_FOUND})
    MESSAGE(STATUS "SolveSpaceSolver plugin found in ${SSPath}")
    ADD_SUBDIRECTORY(SolveSpaceSolver)
  ENDIF()
  IF(${GCSPath_FOUND})
    MESSAGE(STATUS "PlaneGCSSolver plugin found in ${GCSPath}")
    ADD_SUBDIRECTORY(PlaneGCSSolver)
  ENDIF()
ELSE()
  MESSAGE(WARNING "No sketch solver plugin is found")
ENDIF()
