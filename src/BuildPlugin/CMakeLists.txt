## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(UnitTest)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Events
                    ${PROJECT_SOURCE_DIR}/src/Config
                    ${PROJECT_SOURCE_DIR}/src/ModelAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomAlgoAPI
                    ${PROJECT_SOURCE_DIR}/src/GeomValidators
)

SET(PROJECT_HEADERS
    BuildPlugin.h
    BuildPlugin_Plugin.h
    BuildPlugin_Vertex.h
    BuildPlugin_Edge.h
    BuildPlugin_Wire.h
    BuildPlugin_Face.h
    BuildPlugin_Shell.h
    BuildPlugin_Validators.h
)

SET(PROJECT_SOURCES
    BuildPlugin_Plugin.cpp
    BuildPlugin_Vertex.cpp
    BuildPlugin_Edge.cpp
    BuildPlugin_Wire.cpp
    BuildPlugin_Face.cpp
    BuildPlugin_Shell.cpp
    BuildPlugin_Validators.cpp
)

SET(XML_RESOURCES
    plugin-Build.xml
    vertex_widget.xml
    edge_widget.xml
    wire_widget.xml
    face_widget.xml
    shell_widget.xml
)

SET(PROJECT_LIBRARIES
    Events
    Config
    ModelAPI
    GeomAPI
    GeomAlgoAPI
    GeomValidators
)

ADD_DEFINITIONS(-DBUILDPLUGIN_EXPORTS)
ADD_LIBRARY(BuildPlugin MODULE ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(BuildPlugin ${PROJECT_LIBRARIES})

INSTALL(TARGETS BuildPlugin DESTINATION ${SHAPER_INSTALL_PLUGIN_FILES})
INSTALL(FILES ${XML_RESOURCES} DESTINATION ${SHAPER_INSTALL_XML_RESOURCES})
INSTALL(DIRECTORY icons/ DESTINATION ${SHAPER_INSTALL_XML_RESOURCES}/icons/Build)

# ADD_UNIT_TESTS()
