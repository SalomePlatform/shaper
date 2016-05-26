## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

include(GTest)

include_directories(
  ${PROJECT_SOURCE_DIR}/src/Events
  ${PROJECT_SOURCE_DIR}/src/ModelAPI
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI
  ${PROJECT_SOURCE_DIR}/src/ConstructionAPI
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI/Mock
)

set(PROJECT_SOURCES
  TestPoint.cpp
)

set(PROJECT_LIBRARIES
  ModelAPI
  ModelHighAPI
  ConstructionAPI
)

set(PACKAGE_NAME "ConstructionAPI")
set(TARGET_NAME "Test${PACKAGE_NAME}")

add_executable(${TARGET_NAME} ${PROJECT_SOURCES})
target_link_libraries(${TARGET_NAME} ${PROJECT_LIBRARIES} ${GTEST_LIBRARY})

GTEST_ADD_TESTS(${CMAKE_BINARY_DIR}/bin/${TARGET_NAME} "" ${PROJECT_SOURCES})
