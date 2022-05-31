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

#
# SHAPER unit test system
# How to use:
# INCLUDE(UnitTest)
# ...
# ADD_UNIT_TESTS(TestUndoRedo.py
#                TestResultBody.py)
# or
# SET(PROJECT_UNIT_TESTS
#     TestUndoRedo.py
#     TestWhatever.py
#    )
# ADD_UNIT_TESTS(${PROJECT_UNIT_TESTS})
#
# Where "TestUndoRedo.py" and "TestResultBody.py" are names
# of python test scripts in the "./Test" directory.
# This macro will add "Subproject_" as prefix and
# remove ".py" for the test name automaticaly.
#
# Start building of the "RUN_TESTS" project in the Visual studio
# or run "make test" on linux stations to start the testing procedure.
#

ENABLE_TESTING()

function(GENERATE_TESTS PATH testdir TESTS)
  SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)

  foreach(eachFileName ${ARGN})
    # Strip the ".py" suffix
    GET_FILENAME_COMPONENT(aTestName ${eachFileName} NAME_WE)

    # Add "SubprojectName_" prefix
    GET_FILENAME_COMPONENT(aSubprojectName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    SET(aTestName "${aSubprojectName}_${aTestName}")

    # Full path to the python test file beeing executed
    SET(aTestFileName "${testdir}/${eachFileName}")
    IF(NOT EXISTS ${aTestFileName})
      SET(aTestFileName "${testdir}/Test/${eachFileName}")
    ENDIF(NOT EXISTS ${aTestFileName})
    IF(EXISTS ${aTestFileName})
      ADD_TEST(NAME ${aTestName}
               COMMAND ${PYTHON_EXECUTABLE} ${aTestFileName})
      SET_TESTS_PROPERTIES(${aTestName} PROPERTIES
               ENVIRONMENT "${tests_env};SHAPER_UNIT_TEST_IN_PROGRESS=1"
               LABELS "${aSubprojectName}")
      # Debug output...
      #MESSAGE(STATUS "Test added: ${aTestName} file: ${aTestFileName}")
    ELSE(EXISTS ${aTestFileName})
      MESSAGE(WARNING "Can not find the test file: ${eachFileName}")
      MESSAGE(STATUS "Search paths are: ${testdir}")
      MESSAGE(STATUS "                  ${testdir}/Test")
    ENDIF(EXISTS ${aTestFileName})
  endforeach(eachFileName ${ARGN})
endfunction(GENERATE_TESTS)

function(ADD_UNIT_TESTS)
  GENERATE_TESTS(PATH "${CMAKE_CURRENT_SOURCE_DIR}" TESTS ${ARGN})
endfunction(ADD_UNIT_TESTS)

function(ADD_UNIT_TESTS_API)
  GENERATE_TESTS(PATH "${CMAKE_CURRENT_SOURCE_DIR}" TESTS ${ARGN})
endfunction(ADD_UNIT_TESTS_API)


function(ADD_RESTRICTED_TESTS)
  SET(RESTRICTED_ROOT_DIR $ENV{RESTRICTED_ROOT_DIR} CACHE PATH "Path to the restricted repository")

  if (EXISTS ${RESTRICTED_ROOT_DIR})
    GENERATE_TESTS(PATH "${RESTRICTED_ROOT_DIR}/SHAPER/bugs" TESTS ${ARGN})
  else()
    message(WARNING "RESTRICTED_ROOT_DIR is not specified to run restricted tests")
  endif()
endfunction(ADD_RESTRICTED_TESTS)
