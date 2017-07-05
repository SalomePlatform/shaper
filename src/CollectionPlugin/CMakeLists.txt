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
INCLUDE(UseQtExt)

# additional include directories
INCLUDE_DIRECTORIES(${QT_INCLUDES})

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(PROJECT_HEADERS
    CollectionPlugin.h
    CollectionPlugin_Plugin.h
    CollectionPlugin_Group.h
    CollectionPlugin_Field.h
    CollectionPlugin_WidgetCreator.h
    CollectionPlugin_WidgetField.h
)

SET(PROJECT_MOC_HEADERS
    CollectionPlugin_WidgetField.h
)

SET(PROJECT_SOURCES
    CollectionPlugin_Plugin.cpp
    CollectionPlugin_Group.cpp
    CollectionPlugin_Field.cpp
    CollectionPlugin_WidgetCreator.cpp
    CollectionPlugin_WidgetField.cpp
)

SET(XML_RESOURCES
  plugin-Collection.xml
  group_widget.xml
)

SET(TEXT_RESOURCES
    CollectionPlugin_msg_en.ts
)

# sources / moc wrappings
QT_WRAP_MOC(PROJECT_AUTOMOC ${PROJECT_MOC_HEADERS})

#QT5_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})

SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES} ${QM_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})


INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../GeomValidators
  ../Events
  ../ModuleBase
  ../Config
  ${CAS_INCLUDE_DIRS}
)

SET(PROJECT_LIBRARIES
    Events
    ModelAPI
    GeomAPI
    GeomAlgoAPI
    GeomValidators
    ModuleBase
)

ADD_DEFINITIONS(-DCOLLECTIONPLUGIN_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(CollectionPlugin MODULE
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
            ${XML_RESOURCES}
            ${TEXT_RESOURCES}
            ${PROJECT_AUTOMOC})
TARGET_LINK_LIBRARIES(CollectionPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS CollectionPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Collection)
INSTALL(FILES ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

ADD_UNIT_TESTS(
               TestGroup.py
               TestField.py
               TestGroup1799.py
               TestGroupMove.py
               TestGroupMove2.py
)
