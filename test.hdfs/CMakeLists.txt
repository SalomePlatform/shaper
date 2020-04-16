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

ENABLE_TESTING()

SET(RESTRICTED_ROOT_DIR $ENV{RESTRICTED_ROOT_DIR} CACHE PATH "Path to the restricted repository")

set(hdfFilesRestr "")
if (EXISTS ${RESTRICTED_ROOT_DIR})
  file(GLOB hdfFilesRestr "${RESTRICTED_ROOT_DIR}/SHAPER/test.hdfs/*.hdf")
endif()

file(GLOB hdfFilesCur "${CMAKE_CURRENT_SOURCE_DIR}/*.hdf")
set(hdfFilesRestr ${hdfFilesCur} ${hdfFilesRestr})

if (WIN32) # different separators and path to libraries variable name
  SET(_JUSTPATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_BIN};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES};${SUIT_LIB_DIR};$ENV{PATH}")
  STRING(REPLACE "\\" "/" _JUSTPATH "${_JUSTPATH}")
  STRING(REPLACE ";" "\\;" _JUSTPATH "${_JUSTPATH}")
  SET(_PYTHONPATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES};${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_ADDONS};$ENV{PYTHONPATH}")
  STRING(REPLACE "\\" "/" _PYTHONPATH "${_PYTHONPATH}")
  STRING(REPLACE ";" "\\;" _PYTHONPATH "${_PYTHONPATH}")
else()
  SET(_LD_LIBRARY_PATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_BIN}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES}:${SUIT_LIB_DIR}:$ENV{LD_LIBRARY_PATH}")
  SET(_PYTHONPATH "${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_SWIG}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_PLUGIN_FILES}:${CMAKE_INSTALL_PREFIX}/${SHAPER_INSTALL_ADDONS}:$ENV{PYTHONPATH}")
endif()

foreach(eachFilePath ${hdfFilesRestr})
  # Strip the ".hdf" suffix
  GET_FILENAME_COMPONENT(aTestName ${eachFilePath} NAME_WE)
  # Check corresponding ".py" file with reference data exists
  IF(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${aTestName}.py")
    MESSAGE(WARNING "File ${aTestName}.py containing reference data does not exist")
    continue()
  ENDIF()

  # Add "SubprojectName_" prefix
  GET_FILENAME_COMPONENT(aSubprojectName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  SET(aTestName "${aSubprojectName}_${aTestName}")

  # Full path to the python test file being executed
  SET(aTestFilePath "${eachFilePath}")
  IF(EXISTS ${aTestFilePath})
    ADD_TEST(NAME ${aTestName} COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/test.py" "$ENV{KERNEL_ROOT_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" "${aTestFilePath}")
    if (WIN32) # different path to libraries variable name
      SET_TESTS_PROPERTIES(${aTestName} PROPERTIES ENVIRONMENT "PATH=${_JUSTPATH};PYTHONPATH=${_PYTHONPATH}"
        LABELS "models_hdf")
    else()
      SET_TESTS_PROPERTIES(${aTestName} PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${_LD_LIBRARY_PATH};PYTHONPATH=${_PYTHONPATH}"
        LABELS "models_hdf")
    endif()
    # Debug output...
    # MESSAGE(STATUS "Test added: ${aTestName} file: ${aTestFilePath}")
  ELSE(EXISTS ${aTestFilePath})
    MESSAGE(WARNING "Can not find the test file: ${aTestFilePath}")
  ENDIF(EXISTS ${aTestFilePath})
endforeach(eachFilePath ${ARGN})

ADD_CUSTOM_TARGET(run_hdf_tests COMMAND ${CMAKE_CTEST_COMMAND} -C "${CMAKE_BUILD_TYPE}" -L "models_hdf")
