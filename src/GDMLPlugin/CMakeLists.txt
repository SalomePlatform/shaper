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

SET(CMAKE_AUTOMOC ON)

SET(PROJECT_HEADERS
    GDMLPlugin.h
    GDMLPlugin_Plugin.h
    GDMLPlugin_ConeSegment.h
    GDMLPlugin_Ellipsoid.h
)

SET(PROJECT_SOURCES
    GDMLPlugin_Plugin.cpp
    GDMLPlugin_ConeSegment.cpp
    GDMLPlugin_Ellipsoid.cpp
)

SET(XML_RESOURCES
  plugin-GDML.xml
  conesegment_widget.xml
  ellipsoid_widget.xml
)

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../Events
  ../Config
  ../ModuleBase
  ${OpenCASCADE_INCLUDE_DIR}
)

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    GeomAPI
    GeomAlgoAPI
    ModuleBase
    ${QT_LIBRARIES}
)

ADD_DEFINITIONS(-DGDMLPLUGIN_EXPORTS)
ADD_LIBRARY(GDMLPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(GDMLPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS GDMLPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/GDML)

ADD_UNIT_TESTS()

