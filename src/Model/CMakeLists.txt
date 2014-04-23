INCLUDE(Common)
INCLUDE(FindCAS)

SET(PROJECT_HEADERS
    Model.h
    Model_Application.h
    Model_Document.h
    Model_PluginManager.h
    Model_Data.h
    Model_Iterator.h
    Model_AttributeDouble.h
    Model_AttributeDocRef.h
    Model_Events.h
)

SET(PROJECT_SOURCES
    Model_Application.cpp
    Model_Document.cpp
    Model_PluginManager.cpp
    Model_Data.cpp
    Model_Iterator.cpp
    Model_AttributeDouble.cpp
    Model_AttributeDocRef.cpp
    Model_Events.cpp
)

ADD_DEFINITIONS(-DMODEL_EXPORTS ${CAS_DEFINITIONS} ${BOOST_DEFINITIONS})
ADD_LIBRARY(Model SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(Model ${PROJECT_LIBRARIES} ${CAS_OCAF} ModelAPI Event Config)

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../Event
  ../Config
  ${CAS_INCLUDE_DIRS}
)

INSTALL(TARGETS Model DESTINATION bin)
