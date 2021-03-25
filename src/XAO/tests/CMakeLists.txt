# Copyright (C) 2012-2021  CEA/DEN, EDF R&D
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

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${PTHREAD_INCLUDE_DIR}
  ${OpenCASCADE_INCLUDE_DIR}
  ${CPPUNIT_INCLUDE_DIRS}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${OpenCASCADE_DEFINITIONS}
  ${CPPUNIT_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${CPPUNIT_LIBRARIES}
  XAOShaper
  )

# --- sources ---

SET(TestXAO_SOURCES
  BrepGeometryTest.cxx
  FieldTest.cxx
  GeometryTest.cxx
  GroupTest.cxx
  ImportExportTest.cxx
  XAOTests.cxx
  XaoTest.cxx
  XaoUtilsTest.cxx
  )

# --- rules ---

ADD_EXECUTABLE(TestXAO ${TestXAO_SOURCES})
TARGET_LINK_LIBRARIES(TestXAO ${_link_LIBRARIES})

SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)

ADD_TEST(TestXAO TestXAO)
SET_TESTS_PROPERTIES(TestXAO PROPERTIES ENVIRONMENT "GEOM_SRC_DIR=${PROJECT_SOURCE_DIR};${tests_env};SHAPER_UNIT_TEST_IN_PROGRESS=1")

INSTALL(TARGETS TestXAO DESTINATION ${SALOME_INSTALL_BINS})
