# Copyright (C) 2014-2025  CEA, EDF
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

ENABLE_TESTING()

file(GLOB pyFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.py")

SET(RESTRICTED_ROOT_DIR $ENV{RESTRICTED_ROOT_DIR} CACHE PATH "Path to the restricted repository")

if (EXISTS ${RESTRICTED_ROOT_DIR})
  file(GLOB pyFilesRestr "${RESTRICTED_ROOT_DIR}/SHAPER/test.models/*.py")
endif()

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

foreach(eachFilePath ${pyFiles};${pyFilesRestr})
  # Strip the ".py" suffix
  GET_FILENAME_COMPONENT(aTestName ${eachFilePath} NAME_WE)

  # Add "SubprojectName_" prefix
  GET_FILENAME_COMPONENT(aSubprojectName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  SET(aTestName "${aSubprojectName}_${aTestName}")

  # Full path to the python test file beeing executed
  SET(aTestFilePath "${eachFilePath}")
  IF(EXISTS ${aTestFilePath})
    ADD_TEST(NAME ${aTestName} COMMAND ${PYTHON_EXECUTABLE} ${aTestFilePath})
    if (WIN32) # different path to libraries variable name
      SET_TESTS_PROPERTIES(${aTestName} PROPERTIES ENVIRONMENT "PATH=${_JUSTPATH};PYTHONPATH=${_PYTHONPATH};SHAPER_UNIT_TEST_IN_PROGRESS=1"
        LABELS "models_tests")
    else()
      SET_TESTS_PROPERTIES(${aTestName} PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${_LD_LIBRARY_PATH};PYTHONPATH=${_PYTHONPATH};SHAPER_UNIT_TEST_IN_PROGRESS=1"
        LABELS "models_tests")
    endif()
    # Debug output...
    # MESSAGE(STATUS "Test added: ${aTestName} file: ${aTestFilePath}")
  ELSE(EXISTS ${aTestFilePath})
    MESSAGE(WARNING "Can not find the test file: ${aTestFilePath}")
  ENDIF(EXISTS ${aTestFilePath})
endforeach(eachFilePath ${ARGN})

ADD_CUSTOM_TARGET(run_models_tests COMMAND ${CMAKE_CTEST_COMMAND} -C "${CMAKE_BUILD_TYPE}" -L "models_tests")

include(tests.set)

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/TestModels")
  
  install(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)
  install(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})

  set(TMP_TESTS_NAMES)
  foreach(tfile ${TEST_NAMES})
    list(APPEND TMP_TESTS_NAMES ${CMAKE_CURRENT_SOURCE_DIR}/${tfile})
  endforeach(tfile ${TEST_NAMES})

  install(FILES ${TMP_TESTS_NAMES} DESTINATION ${TEST_INSTALL_DIRECTORY})
  set(TMP_UNSTABLE_TESTS)
  foreach(tfile ${UNSTABLE_TESTS})
    list(APPEND TMP_UNSTABLE_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/${tfile})
  endforeach(tfile ${UNSTABLE_TESTS})

  install(FILES ${TMP_UNSTABLE_TESTS} ${TMP_TESTS_NAMES} DESTINATION ${TEST_INSTALL_DIRECTORY})
endif(${HAVE_SALOME})
