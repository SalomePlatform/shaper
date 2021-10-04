# Copyright (C) 2021  CEA/DEN, EDF R&D
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

include(tests.set)

ADD_UNIT_TESTS(${TEST_NAMES})

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/ExchangePlugin")
  
  install(FILES CTestTestfileInstall.cmake
          DESTINATION ${TEST_INSTALL_DIRECTORY}
          RENAME CTestTestfile.cmake)
  install(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})
  
  set(TMP_TESTS_NAMES)
  foreach(tfile ${TEST_NAMES})
    list(APPEND TMP_TESTS_NAMES "${tfile}")
  endforeach(tfile ${TEST_NAMES})
  
  # tests which require SALOME GUI
  include(tests_gui.set)
  SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)
  set(COMPONENT_NAME "ExchangePlugin")
  foreach(tfile ${TEST_WITH_GUI_NAMES})
    # Strip the ".py" suffix
    get_filename_component(TEST_NAME ${tfile} NAME_WE)
    # -B ${CMAKE_SOURCE_DIR}/doc/salome/examples/testme.py
    set(TEST_NAME ${COMPONENT_NAME}_${TEST_NAME})
    add_test(NAME ${TEST_NAME}
             COMMAND ${PYTHON_EXECUTABLE} -B ${CMAKE_CURRENT_SOURCE_DIR}/testme.py ${CMAKE_CURRENT_SOURCE_DIR}/${tfile})
    set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "${tests_env};SHAPER_UNIT_TEST_IN_PROGRESS=1")
    set_tests_properties(${TEST_NAME} PROPERTIES LABELS "${COMPONENT_NAME}")

    # salome test
    list(APPEND TMP_TESTS_NAMES "${tfile}")
  endforeach()

  install(FILES ${TMP_TESTS_NAMES} DESTINATION ${TEST_INSTALL_DIRECTORY})

  install(FILES testme.py DESTINATION ${TEST_INSTALL_DIRECTORY}
                          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                      GROUP_READ GROUP_EXECUTE
                                      WORLD_READ WORLD_EXECUTE)

  install(FILES tests_gui.set DESTINATION ${TEST_INSTALL_DIRECTORY})
  install(DIRECTORY data DESTINATION ${TEST_INSTALL_DIRECTORY})
endif(${HAVE_SALOME})
