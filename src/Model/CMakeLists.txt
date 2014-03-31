CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

INCLUDE(Common)
INCLUDE(FindCAS)
INCLUDE(FindBoost)

SET(PROJECT_HEADERS
    Model.h
    Model_Document.h
    Model_PluginManager.h
    Model_Feature.h
)

SET(PROJECT_SOURCES
    Model_Document.cxx
    Model_PluginManager.cxx
    Model_Feature.cxx
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
