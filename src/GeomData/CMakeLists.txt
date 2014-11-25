INCLUDE(Common)

SET(PROJECT_HEADERS
    GeomData.h
    GeomData_Point.h
    GeomData_Dir.h
    GeomData_Point2D.h
)

SET(PROJECT_SOURCES
    GeomData_Point.cpp
    GeomData_Dir.cpp
    GeomData_Point2D.cpp
)

SET(PROJECT_LIBRARIES
    ModelAPI 
    GeomAPI 
    Events
    ${CAS_OCAF} 
    ${CAS_SHAPE}
)

ADD_DEFINITIONS(-DGEOMDATA_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(GeomData SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(GeomData ${PROJECT_LIBRARIES})

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../GeomDataAPI
  ../GeomAPI
  ../Events
  ../Config
  ../Model
  ${CAS_INCLUDE_DIRS}
)

INSTALL(TARGETS GeomData DESTINATION bin)
