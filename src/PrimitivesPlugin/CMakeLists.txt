# Copyright (C) 2015-2016 CEA/DEN, EDF R&D

# File:        CMakeLists.txt
# Created:     07 Apr 2016
# Author:      CEA (delegation to Alyotech)

INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    PrimitivesPlugin.h
    PrimitivesPlugin_Plugin.h
    PrimitivesPlugin_Box.h
)

SET(PROJECT_SOURCES
    PrimitivesPlugin_Plugin.cpp
    PrimitivesPlugin_Box.cpp
)

SET(XML_RESOURCES
  plugin-Primitives.xml
  box_widget.xml
)

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
ADD_LIBRARY(PrimitivesPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(PrimitivesPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS PrimitivesPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Primitives)

ADD_UNIT_TESTS(UnitTestBox.py
               APIDirectTestBox.py
)

