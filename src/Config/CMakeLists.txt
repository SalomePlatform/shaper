## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)
INCLUDE(XMLProcessing)

INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/Events
                     ${PYTHON_INCLUDE_DIR}) 

SET(PROJECT_HEADERS
  Config_def.h
  Config_FeatureMessage.h
  Config_XMLReader.h
  Config_ModuleReader.h
  Config_FeatureReader.h
  Config_Keywords.h
  Config_WidgetAPI.h
  Config_WidgetReader.h
  Config_PointerMessage.h
  Config_Common.h
  Config_ValidatorMessage.h
  Config_Prop.h
  Config_PropManager.h
  Config_AttributeMessage.h
  Config_SelectionFilterMessage.h
  Config_ValidatorReader.h
  Config_DataModelReader.h
 )
 
SET(PROJECT_SOURCES
  Config_FeatureMessage.cpp
  Config_XMLReader.cpp
  Config_ModuleReader.cpp
  Config_FeatureReader.cpp
  Config_WidgetAPI.cpp
  Config_WidgetReader.cpp
  Config_PointerMessage.cpp
  Config_Common.cpp
  Config_ValidatorMessage.cpp
  Config_Prop.cpp
  Config_PropManager.cpp
  Config_AttributeMessage.cpp
  Config_SelectionFilterMessage.cpp
  Config_ValidatorReader.cpp
  Config_DataModelReader.cpp
)

SET(XML_RESOURCES
  plugins.xml
  dataModel.xml
)

SET(PROJECT_LIBRARIES
    Events
    ${LIBXML2_LIBRARIES}
    ${PYTHON_LIBRARIES}
)

SOURCE_GROUP ("Resource Files" FILES ${XML_RESOURCES})

ADD_DEFINITIONS(-DCONFIG_EXPORTS -D_SCL_SECURE_NO_WARNINGS) 
# -D_SCL_SECURE_NO_WARNINGS - to disable warnings 4996

ADD_LIBRARY(Config SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${XML_RESOURCES})

TARGET_LINK_LIBRARIES(Config ${PROJECT_LIBRARIES})

INSTALL(TARGETS Config DESTINATION bin)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
