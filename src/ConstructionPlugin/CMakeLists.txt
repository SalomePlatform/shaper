## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    ConstructionPlugin.h
    ConstructionPlugin_Plugin.h
    ConstructionPlugin_Point.h
)

SET(PROJECT_SOURCES
    ConstructionPlugin_Plugin.cpp
    ConstructionPlugin_Point.cpp
)

SET(XML_RESOURCES
  plugin-Construction.xml
  point_widget.xml
)

SET(PROJECT_LIBRARIES
    ModelAPI 
    GeomAPI 
    GeomAlgoAPI
    ${CAS_KERNEL}
    ${CAS_SHAPE}
)

ADD_DEFINITIONS(-DCONSTRUCTIONPLUGIN_EXPORTS)
ADD_LIBRARY(ConstructionPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(ConstructionPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS ConstructionPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
)


ADD_UNIT_TESTS(TestPointName.py
              )
