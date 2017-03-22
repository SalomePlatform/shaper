# Copyright (C) 2014-2016 CEA/DEN, EDF R&D

# File:        CMakeLists.txt
# Created:     07 Apr 2016
# Author:      Clarisse genrault (CEA)

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


