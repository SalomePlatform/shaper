## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(PROJECT_HEADERS
    GeomAlgoImpl.h
    GEOMAlgo_Splitter.hxx
)

SET(PROJECT_SOURCES
    GEOMAlgo_Splitter.cxx
)

SET(PROJECT_LIBRARIES
  ${CAS_TKernel}
  ${CAS_TKBRep}
  ${CAS_TKBO}
  ${CAS_SHAPE}
)

ADD_DEFINITIONS(-DGEOMALGOIMPL_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(GeomAlgoImpl SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(GeomAlgoImpl ${PROJECT_LIBRARIES})

INSTALL(TARGETS GeomAlgoImpl DESTINATION ${SHAPER_INSTALL_BIN})
