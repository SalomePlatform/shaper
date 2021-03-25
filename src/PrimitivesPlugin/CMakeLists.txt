# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

SET(PROJECT_HEADERS
    PrimitivesPlugin.h
    PrimitivesPlugin_Plugin.h
    PrimitivesPlugin_Box.h
    PrimitivesPlugin_Cone.h
    PrimitivesPlugin_Cylinder.h
    PrimitivesPlugin_Sphere.h
    PrimitivesPlugin_Torus.h
)

SET(PROJECT_SOURCES
    PrimitivesPlugin_Plugin.cpp
    PrimitivesPlugin_Box.cpp
    PrimitivesPlugin_Cone.cpp
    PrimitivesPlugin_Cylinder.cpp
    PrimitivesPlugin_Sphere.cpp
    PrimitivesPlugin_Torus.cpp
)

SET(XML_RESOURCES
  plugin-Primitives.xml
  box_widget.xml
  cone_widget.xml
  cylinder_widget.xml
  sphere_widget.xml
  torus_widget.xml
)

SET(TEXT_RESOURCES
    PrimitivesPlugin_msg_fr.ts
)

SOURCE_GROUP ("XML Files" FILES ${XML_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../Events
)

SET(PROJECT_LIBRARIES
    Events
    ModelAPI
    GeomAPI
    GeomAlgoAPI
)

ADD_DEFINITIONS(-DPRIMITIVESPLUGIN_EXPORTS)
ADD_LIBRARY(PrimitivesPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})
TARGET_LINK_LIBRARIES(PrimitivesPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS PrimitivesPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Primitives)


