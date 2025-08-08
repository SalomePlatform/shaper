# Copyright (C) 2014-2025  CEA, EDF
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
    CollectionPlugin_GroupAddition.h
    CollectionPlugin_GroupMerge.h
    CollectionPlugin_GroupShape.h
    CollectionPlugin_GroupIntersection.h
    CollectionPlugin_GroupOperation.h
    CollectionPlugin_GroupSubstraction.h
    CollectionPlugin_Field.h
    CollectionPlugin_WidgetCreator.h
    CollectionPlugin_WidgetField.h
    CollectionPlugin_Validators.h
)

SET(PROJECT_MOC_HEADERS
    CollectionPlugin_WidgetField.h
)

SET(PROJECT_SOURCES
    CollectionPlugin_Plugin.cpp
    CollectionPlugin_Group.cpp
    CollectionPlugin_GroupAddition.cpp
    CollectionPlugin_GroupMerge.cpp
    CollectionPlugin_GroupShape.cpp
    CollectionPlugin_GroupIntersection.cpp
    CollectionPlugin_GroupOperation.cpp
    CollectionPlugin_GroupSubstraction.cpp
    CollectionPlugin_Field.cpp
    CollectionPlugin_WidgetCreator.cpp
    CollectionPlugin_WidgetField.cpp
    CollectionPlugin_Validators.cpp
)

SET(XML_RESOURCES
  plugin-Collection.xml
  group_widget.xml
  group_addition_widget.xml
  group_intersection_widget.xml
  group_substraction_widget.xml
)

SET(TEXT_RESOURCES
    CollectionPlugin_msg_en.ts
    CollectionPlugin_msg_fr.ts
)

# sources / moc wrappings
QT_WRAP_MOC(PROJECT_AUTOMOC ${PROJECT_MOC_HEADERS})

#QT5_ADD_TRANSLATION(QM_RESOURCES ${TEXT_RESOURCES})

SOURCE_GROUP ("Generated Files" FILES ${PROJECT_AUTOMOC} ${PROJECT_COMPILED_RESOURCES} ${QM_RESOURCES})
SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})
SOURCE_GROUP ("XML Files" FILES ${XML_RESOURCES})


INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../GeomValidators
  ../Events
  ../ModuleBase
  ../Config
  ${OpenCASCADE_INCLUDE_DIR}
)

SET(PROJECT_LIBRARIES
    Events
    ModelAPI
    GeomAPI
    GeomAlgoAPI
    GeomValidators
    ModuleBase
)

ADD_DEFINITIONS(-DCOLLECTIONPLUGIN_EXPORTS ${OpenCASCADE_DEFINITIONS})
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

include(tests.set)

ADD_UNIT_TESTS(${UNSTABLE_TESTS} ${TEST_NAMES})

if(${HAVE_SALOME})
  enable_testing()
  set(TEST_INSTALL_DIRECTORY "${SALOME_SHAPER_INSTALL_TESTS}/CollectionPlugin")
  
  install(FILES CTestTestfileInstall.cmake
  DESTINATION ${TEST_INSTALL_DIRECTORY}
  RENAME CTestTestfile.cmake)
  install(FILES tests.set DESTINATION ${TEST_INSTALL_DIRECTORY})
  
  set(TMP_TESTS_NAMES)
  foreach(tfile ${TEST_NAMES})
    list(APPEND TMP_TESTS_NAMES "Test/${tfile}")
  endforeach(tfile ${TEST_NAMES})

  foreach(tfile ${UNSTABLE_TESTS})
    list(APPEND TMP_TESTS_NAMES "Test/${tfile}")
  endforeach(tfile ${UNSTABLE_TESTS})
  
  install(FILES ${TMP_TESTS_NAMES} DESTINATION ${TEST_INSTALL_DIRECTORY})
endif(${HAVE_SALOME})
