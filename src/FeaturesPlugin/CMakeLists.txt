## Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    FeaturesPlugin.h
    FeaturesPlugin_Plugin.h
    FeaturesPlugin_Extrusion.h
    FeaturesPlugin_Boolean.h
    FeaturesPlugin_Group.h
    FeaturesPlugin_Placement.h
)

SET(PROJECT_SOURCES
    FeaturesPlugin_Plugin.cpp
    FeaturesPlugin_Extrusion.cpp
    FeaturesPlugin_Boolean.cpp
    FeaturesPlugin_Group.cpp
    FeaturesPlugin_Placement.cpp
)

SET(XML_RESOURCES
  plugin-Features.xml
  extrusion_widget.xml
  boolean_widget.xml
  group_widget.xml
  placement_widget.xml
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

ADD_DEFINITIONS(-DFEATURESPLUGIN_EXPORTS)
ADD_LIBRARY(FeaturesPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(FeaturesPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS FeaturesPlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)

ADD_UNIT_TESTS(TestExtrusion.py
               TestBoolean.py
               TestGroup.py
               TestMultiBoolean.py)
