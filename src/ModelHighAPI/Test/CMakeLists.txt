## Copyright (C) 2014-20xx CEA/DEN, EDF R&D

include(GTest)

set(PROJECT_SOURCES
  TestDouble.cpp
  TestSelection.cpp
)

include_directories(
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI
  ${PROJECT_SOURCE_DIR}/src/ModelHighAPI/Mock
)

set(PROJECT_LIBRARIES
  ModelHighAPI
)

set(PACKAGE_NAME "ModelHighAPI")
set(TARGET_NAME "Test${PACKAGE_NAME}")

add_executable(${TARGET_NAME} ${PROJECT_SOURCES})
target_link_libraries(${TARGET_NAME} ${PROJECT_LIBRARIES} ${GTEST_LIBRARY})

GTEST_ADD_TESTS(${CMAKE_BINARY_DIR}/bin/${TARGET_NAME} "" ${PROJECT_SOURCES})
