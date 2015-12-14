## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(UnitTest)

SET(PROJECT_HEADERS
    SketchShapePlugin.h
    SketchShapePlugin_Feature.h
    SketchShapePlugin_PageGroupBox.h
    SketchShapePlugin_Plugin.h
    SketchShapePlugin_Validators.h
    SketchShapePlugin_Tools.h
    SketchShapePlugin_WidgetCheckGroupBox.h
    SketchShapePlugin_WidgetCreator.h
    SketchShapePlugin_WidgetMultiSelector.h
)

SET(PROJECT_SOURCES
    SketchShapePlugin_Feature.cpp
    SketchShapePlugin_PageGroupBox.cpp
    SketchShapePlugin_Plugin.cpp
    SketchShapePlugin_Validators.cpp
    SketchShapePlugin_Tools.cpp
    SketchShapePlugin_WidgetCheckGroupBox.cpp
    SketchShapePlugin_WidgetCreator.cpp
    SketchShapePlugin_WidgetMultiSelector.cpp
)

SET(PROJECT_LIBRARIES
    Config
    GeomAPI
    GeomAlgoAPI
    ModelAPI
    GeomDataAPI
    ModuleBase
    ${QT_LIBRARIES}
)

SET(XML_RESOURCES
  plugin-SketchShape.xml
)

ADD_DEFINITIONS(-DSKETCHSHAPEPLUGIN_EXPORTS -DWNT)
ADD_LIBRARY(SketchShapePlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})
TARGET_LINK_LIBRARIES(SketchShapePlugin ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ../Config
  ../Events
  ../ModelAPI
  ../GeomAPI
  ../GeomAlgoAPI
  ../GeomDataAPI
  ../ModuleBase
  ${SUIT_INCLUDE}
  ${CAS_INCLUDE_DIRS}
)

INSTALL(TARGETS SketchShapePlugin DESTINATION plugins)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)

ADD_UNIT_TESTS(TestSketchShape.py)
