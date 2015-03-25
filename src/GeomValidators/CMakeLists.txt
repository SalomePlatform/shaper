## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)

SET(PROJECT_HEADERS
    GeomValidators.h
    GeomValidators_ConstructionComposite.h
    GeomValidators_Edge.h
    GeomValidators_EdgeOrVertex.h
    GeomValidators_Face.h
    GeomValidators_Positive.h
    GeomValidators_Tools.h
)

SET(PROJECT_SOURCES
    GeomValidators_ConstructionComposite.cpp
    GeomValidators_Edge.cpp
    GeomValidators_EdgeOrVertex.cpp
    GeomValidators_Face.cpp
    GeomValidators_Positive.cpp
    GeomValidators_Tools.cpp
)

SET(PROJECT_LIBRARIES
    ModelAPI 
    Events
    GeomAPI
)



ADD_DEFINITIONS(-DGEOMVALIDATORS_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(GeomValidators SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(GeomValidators ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/ModelAPI
  ${CMAKE_SOURCE_DIR}/src/Events
  ${CMAKE_SOURCE_DIR}/src/GeomAPI
  ${CMAKE_SOURCE_DIR}/src/GeomDataAPI
)

INSTALL(TARGETS GeomValidators DESTINATION plugins)
