CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

INCLUDE(Common)
INCLUDE(FindCAS)

SET(PROJECT_HEADERS
    Model.h
    Model_Application.h
    Model_Document.h
    Model_PluginManager.h
    Model_Object.h
    Model_Iterator.h
    Model_AttributeDocRef.h
)

SET(PROJECT_SOURCES
    Model_Application.cxx
    Model_Document.cxx
    Model_PluginManager.cxx
    Model_Object.cxx
    Model_Iterator.cxx
    Model_AttributeDocRef.cxx
)

ADD_DEFINITIONS(-DMODEL_EXPORTS ${CAS_DEFINITIONS})
ADD_LIBRARY(Model SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})
TARGET_LINK_LIBRARIES(Model ${PROJECT_LIBRARIES} ${CAS_OCAF} ModelAPI Event Config)

INCLUDE_DIRECTORIES(
  ../ModelAPI
  ../Event
  ../Config
  ${CAS_INCLUDE_DIRS}
)
