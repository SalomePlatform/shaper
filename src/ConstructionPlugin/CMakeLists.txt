## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    ConstructionPlugin.h
    ConstructionPlugin_Plugin.h
    ConstructionPlugin_Point.h
	ConstructionPlugin_Axis.h
	ConstructionPlugin_Plane.h
)

SET(PROJECT_SOURCES
    ConstructionPlugin_Plugin.cpp
    ConstructionPlugin_Point.cpp
	ConstructionPlugin_Axis.cpp
	ConstructionPlugin_Plane.cpp
)

SET(XML_RESOURCES
  plugin-Construction.xml
  point_widget.xml
  axis_widget.xml
  plane_widget.xml
)

SET(PROJECT_LIBRARIES
    Config
    ModelAPI 
    GeomAPI 
    GeomAlgoAPI
)

ADD_DEFINITIONS(-DCONSTRUCTIONPLUGIN_EXPORTS)
ADD_LIBRARY(ConstructionPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(ConstructionPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ConstructionPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)

INCLUDE_DIRECTORIES(
  ../Config
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
)


ADD_UNIT_TESTS(TestAxisCreation.py
               TestPointName.py)
