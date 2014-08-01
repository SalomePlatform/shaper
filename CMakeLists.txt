CMAKE_MINIMUM_REQUIRED(VERSION 2.8.10)

PROJECT (NewGEOM)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeCommon" ${CMAKE_MODULE_PATH})

INCLUDE(Common)
INCLUDE(FindQt)
INCLUDE(FindBoost)
INCLUDE(FindPython)
INCLUDE(FindSalome)
INCLUDE(FindSolveSpace)
INCLUDE(FindCAS)

IF(NOT ${HAVE_SALOME})
	INCLUDE(FindSUIT)
ENDIF(NOT ${HAVE_SALOME})

IF(UNIX)
  IF(CMAKE_COMPILER_IS_GNUCC)
    #C++11 is not supported on some platforms, disable it 
    MESSAGE(STATUS "Setting -std=c++0x flag for the gcc...")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    
    #Supporting test coverage checks (gcov) in the DEBUG mode
    IF(USE_TEST_COVERAGE)
      INCLUDE(CodeCoverage)
      MESSAGE(STATUS "Setting flags for gcov support the the gcc...")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
      SET(CMAKE_SHARED_LINKER_FLAGS "-lgcov")
      
      SETUP_TARGET_FOR_COVERAGE(test_coverage  # Name for custom target.
				                ctest          # Name of the test driver executable that runs the tests.
				                coverage)      # Name of output directory.
    ENDIF(USE_TEST_COVERAGE)
    
    #SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -E")
    MESSAGE(STATUS "gcc flags are: " ${CMAKE_CXX_FLAGS})
    MESSAGE(STATUS "linker flags are: " ${CMAKE_SHARED_LINKER_FLAGS})
  ENDIF(CMAKE_COMPILER_IS_GNUCC)
ENDIF(UNIX)


ADD_SUBDIRECTORY (src/Config)
ADD_SUBDIRECTORY (src/Events)
ADD_SUBDIRECTORY (src/Model)
ADD_SUBDIRECTORY (src/ModelAPI)
ADD_SUBDIRECTORY (src/GeomAPI)
ADD_SUBDIRECTORY (src/GeomAlgoAPI)
ADD_SUBDIRECTORY (src/GeomData)
ADD_SUBDIRECTORY (src/GeomDataAPI)
ADD_SUBDIRECTORY (src/PartSetPlugin)
ADD_SUBDIRECTORY (src/ConstructionPlugin)
ADD_SUBDIRECTORY (src/FeaturesPlugin)
ADD_SUBDIRECTORY (src/SketchPlugin)
ADD_SUBDIRECTORY (src/SketchSolver)
ADD_SUBDIRECTORY (src/ModuleBase)
ADD_SUBDIRECTORY (src/PartSet)
ADD_SUBDIRECTORY (src/XGUI)
ADD_SUBDIRECTORY (src/GeomApp)

IF(${HAVE_SALOME})
	ADD_SUBDIRECTORY (src/NewGeom)
ENDIF(${HAVE_SALOME})

ENABLE_TESTING()
