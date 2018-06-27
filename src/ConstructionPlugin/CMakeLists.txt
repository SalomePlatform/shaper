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
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    ConstructionPlugin.h
    ConstructionPlugin_Plugin.h
    ConstructionPlugin_Point.h
    ConstructionPlugin_Axis.h
    ConstructionPlugin_Plane.h
    ConstructionPlugin_Validators.h
)

SET(PROJECT_SOURCES
    ConstructionPlugin_Plugin.cpp
    ConstructionPlugin_Point.cpp
    ConstructionPlugin_Axis.cpp
    ConstructionPlugin_Plane.cpp
    ConstructionPlugin_Validators.cpp
)

SET(XML_RESOURCES
  plugin-Construction.xml
  point_widget.xml
  axis_widget.xml
  plane_widget.xml
)

SET(TEXT_RESOURCES
  ConstructionPlugin_msg_en.ts
)

SET(PROJECT_LIBRARIES
    Config
    ModelAPI
    GeomAPI
    GeomAlgoAPI
)

SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

ADD_DEFINITIONS(-DCONSTRUCTIONPLUGIN_EXPORTS)
ADD_LIBRARY(ConstructionPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})
TARGET_LINK_LIBRARIES(ConstructionPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ConstructionPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Construction)

INCLUDE_DIRECTORIES(
  ../Config
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../Events
)


ADD_UNIT_TESTS(TestAxisCreation.py
               UnitTestAxis.py
               TestPoint_XYZ.py
               TestPoint_LineAndPlane.py
               TestPoint_Edge.py
               TestPoint_ProjectOnEdge.py
               TestPoint_ProjectOnFace.py
               TestPointName.py
               TestPlane.py)
