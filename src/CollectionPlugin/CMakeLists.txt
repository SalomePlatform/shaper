## Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    CollectionPlugin.h
    CollectionPlugin_Plugin.h
    CollectionPlugin_Group.h
)

SET(PROJECT_SOURCES
    CollectionPlugin_Plugin.cpp
    CollectionPlugin_Group.cpp
)

SET(XML_RESOURCES
  plugin-Collection.xml
  group_widget.xml
)

SET(TEXT_RESOURCES
    CollectionPlugin_msg_en.ts
)

SOURCE_GROUP ("Resource Files" FILES ${TEXT_RESOURCES})


INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../GeomValidators
  ../Events
)

SET(PROJECT_LIBRARIES
    Events
    ModelAPI
    GeomAPI
    GeomAlgoAPI
    GeomValidators
)

ADD_DEFINITIONS(-DCOLLECTIONPLUGIN_EXPORTS)
ADD_LIBRARY(CollectionPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES} ${TEXT_RESOURCES})
TARGET_LINK_LIBRARIES(CollectionPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS CollectionPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Collection)
INSTALL(FILES ${TEXT_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})

ADD_UNIT_TESTS(
               TestGroup.py
)
