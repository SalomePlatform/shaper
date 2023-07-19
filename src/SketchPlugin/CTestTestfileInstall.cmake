# Copyright (C) 2021-2023  CEA, EDF
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
include(tests_change_radius.set)

set(TEST_NAMES ${TEST_NAMES_PARA} ${TEST_NAMES_CHANGE_RADIUS})

foreach(tfile ${TEST_NAMES})
  set(TEST_NAME ${COMPONENT_NAME}_${tfile})
  add_test(${TEST_NAME} python ${tfile})
  set_tests_properties(${TEST_NAME} PROPERTIES LABELS "${SALOME_TEST_LABEL_ADV}")
endforeach()

foreach(tfile ${TEST_NAMES_SEQ})
  set(TEST_NAME ${COMPONENT_NAME}_${tfile})
  add_test(${TEST_NAME} python ${tfile})
  set_tests_properties(${TEST_NAME} PROPERTIES LABELS "${SALOME_TEST_LABEL_SEQ}")
endforeach()
