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

INCLUDE(Common)

SET(PROJECT_HEADERS
    Selector.h
    Selector_Selector.h
    Selector_NameGenerator.h
)

SET(PROJECT_SOURCES
    Selector_Selector.cpp
    Selector_NameGenerator.cpp
)

SET(PROJECT_LIBRARIES
    ${OpenCASCADE_ApplicationFramework_LIBRARIES}
)
SET(PROJECT_INCLUDES
  ${OpenCASCADE_INCLUDE_DIR}
)


ADD_DEFINITIONS(-DSELECTOR_EXPORTS ${OpenCASCADE_DEFINITIONS})
ADD_LIBRARY(Selector SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(Selector ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(${PROJECT_INCLUDES})

INSTALL(TARGETS Selector DESTINATION ${SHAPER_INSTALL_BIN})
