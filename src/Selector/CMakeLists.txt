# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
    Selector.h
    Selector_Selector.h
    Selector_Algo.h
    Selector_Primitive.h
    Selector_AlgoWithSubs.h
    Selector_Intersect.h
    Selector_Container.h
    Selector_Modify.h
    Selector_FilterByNeighbors.h
    Selector_WeakName.h
    Selector_NameGenerator.h
    Selector_NExplode.h
)

SET(PROJECT_SOURCES
    Selector_Selector.cpp
    Selector_Algo.cpp
    Selector_Primitive.cpp
    Selector_AlgoWithSubs.cpp
    Selector_Intersect.cpp
    Selector_Container.cpp
    Selector_Modify.cpp
    Selector_FilterByNeighbors.cpp
    Selector_WeakName.cpp
    Selector_NameGenerator.cpp
    Selector_NExplode.cpp
)

SET(PROJECT_LIBRARIES
    Locale
    ${OpenCASCADE_ApplicationFramework_LIBRARIES}
)
SET(PROJECT_INCLUDES
  ../Locale
  ${OpenCASCADE_INCLUDE_DIR}
)


ADD_DEFINITIONS(-DSELECTOR_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(Selector SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(Selector ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(${PROJECT_INCLUDES})

INSTALL(TARGETS Selector DESTINATION ${SHAPER_INSTALL_BIN})
