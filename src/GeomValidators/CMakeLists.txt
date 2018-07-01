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
    GeomValidators.h
    GeomValidators_BodyShapes.h
    GeomValidators_ConstructionComposite.h
    GeomValidators_DifferentShapes.h
    GeomValidators_Face.h
    GeomValidators_FeatureKind.h
    GeomValidators_Finite.h
    GeomValidators_PartitionArguments.h
    GeomValidators_Plugin.h
    GeomValidators_Positive.h
    GeomValidators_ShapeType.h
    GeomValidators_Tools.h
    GeomValidators_ZeroOffset.h
    GeomValidators_Different.h
    GeomValidators_IntersectionSelection.h
    GeomValidators_MinObjectsSelected.h
    GeomValidators_ValueOrder.h
)

SET(PROJECT_SOURCES
    GeomValidators_BodyShapes.cpp
    GeomValidators_ConstructionComposite.cpp
    GeomValidators_DifferentShapes.cpp
    GeomValidators_Face.cpp
    GeomValidators_FeatureKind.cpp
    GeomValidators_Finite.cpp
    GeomValidators_PartitionArguments.cpp
    GeomValidators_Plugin.cpp
    GeomValidators_Positive.cpp
    GeomValidators_ShapeType.cpp
    GeomValidators_Tools.cpp
    GeomValidators_ZeroOffset.cpp
    GeomValidators_Different.cpp
    GeomValidators_IntersectionSelection.cpp
    GeomValidators_MinObjectsSelected.cpp
    GeomValidators_ValueOrder.cpp
)

SET(PROJECT_LIBRARIES
    ModelAPI
    Events
    Config
    GeomAPI
)

ADD_DEFINITIONS(-DGEOMVALIDATORS_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(GeomValidators SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(GeomValidators ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/Config
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
)

INSTALL(TARGETS GeomValidators DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
