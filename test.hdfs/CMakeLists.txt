# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

# get all restricted HDF tests
SET(hdfFilesRestr "")
IF (EXISTS $ENV{RESTRICTED_ROOT_DIR})
  FILE(GLOB hdfFilesRestr "$ENV{RESTRICTED_ROOT_DIR}/SHAPER/test.hdfs/*.hdf")
ENDIF()

# get all HDF tests from the current drectory
file(GLOB hdfFilesCur "${CMAKE_CURRENT_SOURCE_DIR}/*.hdf")

# collect test names
set(TEST_NAMES "")
foreach(eachFilePath IN LISTS hdfFilesCur hdfFilesRestr)
  # Strip the ".hdf" suffix
  get_filename_component(aTestName ${eachFilePath} NAME_WE)
  # Check corresponding ".py" file with reference data exists
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${aTestName}.py")
    set(TEST_NAMES ${TEST_NAMES} ${aTestName})
  else()
    message(WARNING "File ${aTestName}.py containing reference data for ${aTestName}.hdf does not exist")
  endif()
endforeach()

SET(TIMEOUT 600)

SET(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/HDFs")

SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)

SET(PUBLIC_TESTS "")
SET(RESTRICTED_TESTS "")
FOREACH(tfile ${TEST_NAMES})
  SET(TEST_NAME ${COMPONENT_NAME}_HDF_${tfile})

  IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${tfile}.hdf")
    SET(HDF_TEST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${tfile}.hdf")
    SET(PUBLIC_TESTS ${PUBLIC_TESTS} ${tfile})
  ELSEIF(EXISTS "$ENV{RESTRICTED_ROOT_DIR}/SHAPER/test.hdfs/${tfile}.hdf")
    SET(HDF_TEST_FILE "$ENV{RESTRICTED_ROOT_DIR}/SHAPER/test.hdfs/${tfile}.hdf")
    SET(RESTRICTED_TESTS ${RESTRICTED_TESTS} ${tfile})
  ENDIF()

  ADD_TEST(NAME ${TEST_NAME}
           COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/testme.py" "${HDF_TEST_FILE}" "${CMAKE_CURRENT_SOURCE_DIR}/${tfile}.py")
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES ENVIRONMENT "${tests_env};SHAPER_UNIT_TEST_IN_PROGRESS=1")
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES LABELS "${COMPONENT_NAME};models_hdf")
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES TIMEOUT ${TIMEOUT})
ENDFOREACH()

ADD_CUSTOM_TARGET(run_hdf_tests COMMAND ${CMAKE_CTEST_COMMAND} -C "${CMAKE_BUILD_TYPE}" -L "models_hdf")

# salome test
FOREACH(tfile ${TEST_NAMES})
  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${tfile}.py
          DESTINATION ${TEST_INSTALL_DIRECTORY})
  IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${tfile}.hdf")
    INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${tfile}.hdf
            DESTINATION ${TEST_INSTALL_DIRECTORY})
  ENDIF()
ENDFOREACH()

INSTALL(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)

INSTALL(FILES testme.py DESTINATION ${TEST_INSTALL_DIRECTORY}
                        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                    GROUP_READ GROUP_EXECUTE
                                    WORLD_READ WORLD_EXECUTE)
FILE(COPY test_hdf.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
INSTALL(FILES test_hdf.py DESTINATION ${TEST_INSTALL_DIRECTORY}
                          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                      GROUP_READ GROUP_EXECUTE
                                      WORLD_READ WORLD_EXECUTE)
CONFIGURE_FILE(tests.set.in ${CMAKE_CURRENT_BINARY_DIR}/tests.set @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})
