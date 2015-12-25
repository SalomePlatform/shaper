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
  ${CMAKE_CURRENT_BINARY_DIR}/plugins.xml
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

# Prepare plugins.xml

# the solver to activate after installation
SET(DEFAULT_SOLVER "SolveSpace")

# the list of all solvers
SET(SOLVERS "SolveSpace" "PlaneGCS")

# apply NO_<solver> variables
FOREACH(solver IN LISTS SOLVERS)
  IF(NO_${solver})
    LIST(REMOVE_ITEM SOLVERS ${solver})
  ENDIF()
ENDFOREACH()

# check default solver in the list of solvers
LIST(FIND SOLVERS ${DEFAULT_SOLVER} INDEX)
IF(${INDEX} GREATER -1)
  MESSAGE(STATUS "Default solver: ${DEFAULT_SOLVER}") 
ELSE()
  MESSAGE(FATAL_ERROR "Default solver: ${DEFAULT_SOLVER} is switched off.\nUnset NO_${DEFAULT_SOLVER} variable or define another default solver DEFAULT_SOLVER=<SOLVER> from the list: ${SOLVERS}.") 
ENDIF()

# prepare variables for configuration
SET(DEFAULT_SOLVER "  <plugin library=\"${DEFAULT_SOLVER}Solver\"/>")
FOREACH(solver IN LISTS SOLVERS)
  IF(DEFINED ALL_SOLVERS)
    SET(ALL_SOLVERS "${ALL_SOLVERS}\n")
  ENDIF()
  SET(ALL_SOLVERS "${ALL_SOLVERS}  <plugin library=\"${solver}Solver\"/>")
ENDFOREACH()

# configuration
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/plugins.xml.in"
  "${CMAKE_CURRENT_BINARY_DIR}/plugins.xml"
  )

INSTALL(TARGETS Config DESTINATION bin)
INSTALL(FILES ${XML_RESOURCES} DESTINATION plugins)
