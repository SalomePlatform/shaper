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

SET(PYTHON_FILES
    ConnectorPlugin.py
    ConnectorPlugin_ExportFeature.py
    ConnectorPlugin_PublishToStudyFeature.py
)

SET(XML_RESOURCES
  plugin-Connector.xml
)

SET(TEXT_RESOURCES
    ConnectorPlugin_msg_fr.ts
)

SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})

ADD_CUSTOM_TARGET(ConnectorPlugin SOURCES ${PYTHON_FILES} ${XML_RESOURCES} ${TEXT_RESOURCES})

INSTALL(FILES ${PYTHON_FILES} DESTINATION ${SHAPER_INSTALL_PYTHON_FILES})
INSTALL(FILES ${XML_RESOURCES} ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Connector)
