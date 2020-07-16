# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

MACRO(ADD_UNIT_TESTS)
  if (WIN32) # different separators and path to libraries variable name
    SET(_JUSTPATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_BIN};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES};${SUIT_LIB_DIR};${SALOME_KERNEL_LIBDIR};$ENV{PATH}")
    STRING(REPLACE "\\" "/" _JUSTPATH "${_JUSTPATH}")
    STRING(REPLACE ";" "\\;" _JUSTPATH "${_JUSTPATH}")
    SET(_PYTHONPATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_ADDONS};$ENV{PYTHONPATH}")
    STRING(REPLACE "\\" "/" _PYTHONPATH "${_PYTHONPATH}")
    STRING(REPLACE ";" "\\;" _PYTHONPATH "${_PYTHONPATH}")
  else()
    SET(_LD_LIBRARY_PATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_BIN}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES}:${SUIT_LIB_DIR}:${SALOME_KERNEL_LIBDIR}:$ENV{LD_LIBRARY_PATH}")
    SET(_PYTHONPATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_ADDONS}:$ENV{PYTHONPATH}")
  endif()

  foreach(eachFileName ${ARGN})
    # Strip the ".py" suffix
    GET_FILENAME_COMPONENT(aTestName ${eachFileName} NAME_WE)

    # Add "SubprojectName_" prefix
    GET_FILENAME_COMPONENT(aSubprojectName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    SET(aTestName "${aSubprojectName}_${aTestName}")

    # Full path to the python test file beeing executed
    SET(aTestFileName "${CMAKE_CURRENT_SOURCE_DIR}/Test/${eachFileName}")
    IF(EXISTS ${aTestFileName})
      ADD_TEST(NAME ${aTestName}
               COMMAND ${PYTHON_EXECUTABLE} ${aTestFileName})
      if (WIN32) # different path to libraries variable name
        SET_TESTS_PROPERTIES(${aTestName} PROPERTIES
               ENVIRONMENT "PATH=${_JUSTPATH};PYTHONPATH=${_PYTHONPATH};SHAPER_UNIT_TEST_IN_PROGRESS=1"
               LABELS "${aSubprojectName}")
      else()
        SET_TESTS_PROPERTIES(${aTestName} PROPERTIES
               ENVIRONMENT "LD_LIBRARY_PATH=${_LD_LIBRARY_PATH};PYTHONPATH=${_PYTHONPATH};SHAPER_UNIT_TEST_IN_PROGRESS=1"
               LABELS "${aSubprojectName}")
      endif()
      # Debug output...
      #MESSAGE(STATUS "Test added: ${aTestName} file: ${aTestFileName}")
    ELSE(EXISTS ${aTestFileName})
      MESSAGE(WARNING "Can not find the test file: ${aTestFileName}")
    ENDIF(EXISTS ${aTestFileName})
  endforeach(eachFileName ${ARGN})
ENDMACRO(ADD_UNIT_TESTS)

MACRO(ADD_UNIT_TESTS_API)
  if (WIN32) # different separators and path to libraries variable name
    SET(_JUSTPATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_BIN};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES};${SUIT_LIB_DIR};${SALOME_KERNEL_LIBDIR};$ENV{PATH}")
    STRING(REPLACE "\\" "/" _JUSTPATH "${_JUSTPATH}")
    STRING(REPLACE ";" "\\;" _JUSTPATH "${_JUSTPATH}")
    SET(_PYTHONPATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_ADDONS};$ENV{PYTHONPATH}")
    STRING(REPLACE "\\" "/" _PYTHONPATH "${_PYTHONPATH}")
    STRING(REPLACE ";" "\\;" _PYTHONPATH "${_PYTHONPATH}")
  else()
    SET(_LD_LIBRARY_PATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_BIN}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES}:${SUIT_LIB_DIR}:${SALOME_KERNEL_LIBDIR}:$ENV{LD_LIBRARY_PATH}")
    SET(_PYTHONPATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_ADDONS}:$ENV{PYTHONPATH}")
  endif()

  foreach(eachFileName ${ARGN})
    # Strip the ".py" suffix
    GET_FILENAME_COMPONENT(aTestName ${eachFileName} NAME_WE)

    # Add "SubprojectName_" prefix
    GET_FILENAME_COMPONENT(aSubprojectName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    SET(aTestName "${aSubprojectName}_${aTestName}")

    # Full path to the python test file beeing executed
    SET(aTestFileName "${CMAKE_CURRENT_SOURCE_DIR}/${eachFileName}")
    IF(EXISTS ${aTestFileName})
      ADD_TEST(NAME ${aTestName}
               COMMAND ${PYTHON_EXECUTABLE} ${aTestFileName})
      if (WIN32) # different path to libraries variable name
        SET_TESTS_PROPERTIES(${aTestName} PROPERTIES
               ENVIRONMENT "PATH=${_JUSTPATH};PYTHONPATH=${_PYTHONPATH};SHAPER_UNIT_TEST_IN_PROGRESS=1"
               LABELS "${aSubprojectName}")
      else()
        SET_TESTS_PROPERTIES(${aTestName} PROPERTIES
               ENVIRONMENT "LD_LIBRARY_PATH=${_LD_LIBRARY_PATH};PYTHONPATH=${_PYTHONPATH};SHAPER_UNIT_TEST_IN_PROGRESS=1"
               LABELS "${aSubprojectName}")
      endif()
      # Debug output...
      #MESSAGE(STATUS "Test added: ${aTestName} file: ${aTestFileName}")
    ELSE(EXISTS ${aTestFileName})
      MESSAGE(WARNING "Can not find the test file: ${aTestFileName}")
    ENDIF(EXISTS ${aTestFileName})
  endforeach(eachFileName ${ARGN})
ENDMACRO(ADD_UNIT_TESTS_API)
