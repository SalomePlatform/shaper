# Copyright (C) 2012-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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

#IF(SALOME_BUILD_TESTS)
#  ADD_SUBDIRECTORY(tests)
#ENDIF(SALOME_BUILD_TESTS)

# --- options ---

INCLUDE(Common)
INCLUDE(XMLProcessing)

# additional include directories
INCLUDE_DIRECTORIES(${CAS_INCLUDE_DIRS})

# additional preprocessor / compiler flags
ADD_DEFINITIONS(-DXAO_EXPORTS ${CAS_DEFINITIONS})

# libraries to link to
SET(PROJECT_LIBRARIES
  ${LIBXML2_LIBRARIES}
  ${CAS_TKBRep} ${CAS_TKTopAlgo} ${CAS_TKG2d}
  )

# --- headers ---

SET(XAO_HEADERS
  XAO.hxx
  XAO_BooleanField.hxx
  XAO_BooleanStep.hxx
  XAO_BrepGeometry.hxx
  XAO_DoubleField.hxx
  XAO_DoubleStep.hxx
  XAO_Exception.hxx
  XAO_Field.hxx
  XAO_GeometricElement.hxx
  XAO_Geometry.hxx
  XAO_Group.hxx
  XAO_IntegerField.hxx
  XAO_IntegerStep.hxx
  XAO_Step.hxx
  XAO_StringField.hxx
  XAO_StringStep.hxx
  XAO_Xao.hxx
  XAO_XaoExporter.hxx
  XAO_XaoUtils.hxx
  )

# --- sources ---

SET(XAO_SOURCES
  XAO_BooleanField.cxx
  XAO_BooleanStep.cxx
  XAO_BrepGeometry.cxx
  XAO_DoubleField.cxx
  XAO_DoubleStep.cxx
  XAO_Field.cxx
  XAO_GeometricElement.cxx
  XAO_Geometry.cxx
  XAO_Group.cxx
  XAO_IntegerField.cxx
  XAO_IntegerStep.cxx
  XAO_Step.cxx
  XAO_StringField.cxx
  XAO_StringStep.cxx
  XAO_Xao.cxx
  XAO_XaoExporter.cxx
  XAO_XaoUtils.cxx
  )

# --- rules ---

ADD_LIBRARY(XAOShaper SHARED ${XAO_SOURCES} ${XAO_HEADERS})
TARGET_LINK_LIBRARIES(XAOShaper ${PROJECT_LIBRARIES})
INSTALL(TARGETS XAOShaper DESTINATION ${SHAPER_INSTALL_BIN})
