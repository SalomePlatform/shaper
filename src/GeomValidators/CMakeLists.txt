## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

INCLUDE(Common)

SET(PROJECT_HEADERS
    GeomValidators.h
    GeomValidators_BodyShapes.h
    GeomValidators_BooleanArguments.h
    GeomValidators_ConstructionComposite.h
    GeomValidators_DifferentShapes.h
    GeomValidators_Face.h
    GeomValidators_Finite.h
    GeomValidators_PartitionArguments.h
    GeomValidators_Plugin.h
    GeomValidators_Positive.h
    GeomValidators_ShapeType.h
    GeomValidators_Tools.h
    GeomValidators_ZeroOffset.h
    GeomValidators_Different.h
)

SET(PROJECT_SOURCES
    GeomValidators_BodyShapes.cpp
    GeomValidators_BooleanArguments.cpp
    GeomValidators_ConstructionComposite.cpp
    GeomValidators_DifferentShapes.cpp
    GeomValidators_Face.cpp
    GeomValidators_Finite.cpp
    GeomValidators_PartitionArguments.cpp
    GeomValidators_Plugin.cpp
    GeomValidators_Positive.cpp
    GeomValidators_ShapeType.cpp
    GeomValidators_Tools.cpp
    GeomValidators_ZeroOffset.cpp
    GeomValidators_Different.cpp
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
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/GeomAPI
  ${PROJECT_SOURCE_DIR}/src/GeomDataAPI
)

INSTALL(TARGETS GeomValidators DESTINATION plugins)

