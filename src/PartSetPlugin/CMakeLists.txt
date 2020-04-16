# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
    PartSetPlugin.h
    PartSetPlugin_Plugin.h
    PartSetPlugin_Part.h
    PartSetPlugin_Duplicate.h
    PartSetPlugin_Remove.h
)

SET(PROJECT_SOURCES
    PartSetPlugin_Plugin.cpp
    PartSetPlugin_Part.cpp
    PartSetPlugin_Duplicate.cpp
    PartSetPlugin_Remove.cpp
)

SET(XML_RESOURCES
  plugin-PartSet.xml
)

SET(TEXT_RESOURCES
    PartSetPlugin_msg_fr.ts
)

SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

ADD_DEFINITIONS(-DPARTSETPLUGIN_EXPORTS)
ADD_LIBRARY(PartSetPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})
TARGET_LINK_LIBRARIES(PartSetPlugin ${PROJECT_LIBRARIES} ModelAPI)

INCLUDE_DIRECTORIES(
  ../Events
  ../ModelAPI
  ../GeomAPI
)

INSTALL(TARGETS PartSetPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/PartSet)
