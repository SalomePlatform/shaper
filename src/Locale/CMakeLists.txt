# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

INCLUDE(Common)
INCLUDE(UnitTest)

IF(NOT Boost_INCLUDE_DIR)
  FILE(TO_CMAKE_PATH "$ENV{BOOST_ROOT_DIR}" BOOST_ROOT)
  FIND_PACKAGE(Boost REQUIRED)
ENDIF()

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIR}
)

SET(PROJECT_HEADERS
    Locale_def.h
    Locale_Convert.h
)

SET(PROJECT_SOURCES
    Locale_Convert.cpp
)

SET(PROJECT_LIBRARIES
)

ADD_DEFINITIONS(-DLOCALE_EXPORTS)
ADD_LIBRARY(Locale SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(Locale ${PROJECT_LIBRARIES})

INSTALL(TARGETS Locale DESTINATION ${SHAPER_INSTALL_BIN})

include(tests.set)

ADD_UNIT_TESTS(${TEST_NAMES})

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/Locale")
  
  install(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)
  install(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})
  
  set(TMP_TESTS_NAMES)
  foreach(tfile ${TEST_NAMES})
    list(APPEND TMP_TESTS_NAMES "Test/${tfile}")
  endforeach(tfile ${TEST_NAMES})
  
  install(FILES ${TMP_TESTS_NAMES} DESTINATION ${TEST_INSTALL_DIRECTORY})
endif(${HAVE_SALOME})
