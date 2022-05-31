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

INCLUDE(Common)

SET(PROJECT_HEADERS
    GeomData.h
    GeomData_Point.h
    GeomData_Dir.h
    GeomData_Point2D.h
    GeomData_Point2DArray.h
)

SET(PROJECT_SOURCES
    GeomData_Point.cpp
    GeomData_Dir.cpp
    GeomData_Point2D.cpp
    GeomData_Point2DArray.cpp
)

SET(PROJECT_LIBRARIES
    GeomDataAPI
    ModelAPI
    GeomAPI
    Events
    ${OpenCASCADE_ApplicationFramework_LIBRARIES}
)

ADD_DEFINITIONS(-DGEOMDATA_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(GeomData SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(GeomData ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomDataAPI
  ../GeomAPI
  ../Events
  ../Config
  ../Model
  ${OpenCASCADE_INCLUDE_DIR}
)

INSTALL(TARGETS GeomData DESTINATION ${SHAPER_INSTALL_BIN})
